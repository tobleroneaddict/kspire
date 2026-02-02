#include "bundle.h"


void Bundle::free() {
    raw.clear();
    assets.clear();
}

//PRIV Test filename
bool Bundle::compare(const char filename[100], const char* test) {
    size_t i = 0;
    while (i < 100 && filename[i] && test[i]) {
        if (tolower(filename[i]) != tolower(test[i])) return false;
        i++;
    }
    return test[i] == '\0';
}


std::vector<uint8_t> Bundle::load_raw_data(const char* name) {
    std::vector<uint8_t> data;

    if (strlen(name) == 0) {return data;} //Null names not allowed

    Asset a;

    //Find asset
    bool found = false;
    for (Asset &ast : assets) {
        if (compare(ast.name,name)) {
            a = ast;
            found = true;
            break;
        } else {
        }
    }

    if (!found) {
        return data;
    }

    unsigned int i = a.offset;
    while (i < a.offset+a.length) {
        data.push_back(raw[i]);
        i++;
    }

    return data;
}


void Bundle::debug_list_assets() {
    for (Asset a : assets) {
        printf("%s\tSize: ",a.name);
        printf("%ld bytes\n",a.length);
    }
}


//PRIV unzip the asset bundle into raw
int Bundle::unzip_ab(const char* name) {
    FILE *fp;
    gzFile file;
    int n;
    uint8_t buf[128];

    //fopen is a nice way to test the existence without exploding the os???
    fp = fopen(name, "rb");

    if (fp == NULL) return 1;

    file = gzopen(name, "rb");

    //Shouldnt ever hit
    if (!file) { return 2; }

    //Read the gz into a tar container
    while ((n = gzread(file, buf, sizeof(buf))) > 0) {
        raw.insert(raw.end(), buf, buf + n);
    }

    gzclose (file);
    return 0;
}

//PRIV Scout all files
void Bundle::journal() {
    unsigned int i = 0;
    while (1) {
        if (i >= raw.size()) break;

        //Make new
        Asset asset;

        //store NAME
        memcpy(asset.name,&raw[i],100);

        i += 124; //Size header offset

        //store SIZE, its in octal so convert
        char size_buffer[13] = {}; size_buffer[12] = '\0';
        memcpy(size_buffer, &raw[i],12); //STO into buffer
        unsigned int file_length = std::strtoul(size_buffer, nullptr, 8); //octal
        
        asset.length = file_length;

        //advance to file position
        i+= 388;
        asset.offset = i; //Store the file location to the asset

        //store asset
        assets.push_back(asset);

        i += file_length; //go to next file
        //Now theres some padding, so:

        while(raw[i] == 0x00) { i++; }
    }
}

int Bundle::load_asset_bundle(const char* name) {
    //Unzip asset bundle first
    unzip_ab(name);

    //And get the headers
    journal();

    return 0;
}
