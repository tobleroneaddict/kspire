#include "bundle.h"

namespace angel {
    void Bundle::free() {
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

    //Raw data loader
    std::vector<uint8_t> Bundle::load_raw_data(const char* name) {
        std::vector<uint8_t> data;

        gzFile file;

        if (bundle_name.size() == 0) { printf("19348 BUNDLE name cannot be empty!\n"); return data;} //Null names not allowed

        //open gz
        file = gzopen(bundle_name.c_str(), "rb");
        if (!file) { printf("45186 Loader error! gz could not find %s",bundle_name.c_str()); return data; }
        
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


        //might be able to do this without iteration lets see

        data.reserve(a.length);
        data.resize(a.length);

        gzseek(file, a.offset, SEEK_SET);

        int bytes_read = gzread(file, data.data(), a.length);

        if (bytes_read != (int)a.length) {
            printf("13481 DATA Load Error");
        }

        gzclose(file);
        return data;
    }


    void Bundle::debug_list_assets() {
        for (Asset a : assets) {
            printf("%s\tSize: ",a.name);
            printf("%d bytes\n",(int)a.length);
        }
    }

    std::vector<std::string> Bundle::ls(std::string path) {
        std::vector<std::string> full_list;
        
        if (path[path.back()] != '/')
            path.append("/");

        for (Asset a : assets) {
            
            //TODO convert all this shit to std::string
            std::string name = std::string(a.name);

            if (name.rfind(path) == 0) {
                std::string lopped = name.substr(path.size()).c_str();
                unsigned int pos = lopped.find_first_of("/");
                if (pos != std::string::npos)
                    lopped = lopped.substr(0,pos);

                if(!(std::find(full_list.begin(), full_list.end(), lopped) != full_list.end()) && lopped != "")
                    full_list.emplace_back(lopped);
            }
        }

        return full_list;
    }

    int Bundle::load_asset_bundle(const char* name) {
        free();
        gzFile file;
        unsigned int  n = 0;
        uint8_t buf[512];


        printf("Name of : %s\n",name);
        file = gzopen(name, "rb");

        //Return if borked
        if (!file) { return 2; }

        //load_raw_data uses this
        bundle_name = std::string(name);

        //Read the targz's headers only and journal them
        while (1) {

            //Read into buffer
            uint16_t bytes_read = gzread(file, buf, 512);

            
            if (buf[0] == 0 || bytes_read < 512)
            break;
            
            Asset asset{};
            memcpy(asset.name, buf, 100);
            
            char size_buffer[13] = {};
            memcpy(size_buffer, buf + 124, 12);
            asset.length = strtoul(size_buffer, nullptr, 8);
            

            //Store asset data position
            asset.offset = n + 512;
            assets.push_back(asset);

            //Skip the file, no need to store it all at once like before....
            uint32_t skip = (asset.length + 511)  & ~511;
            //I dont know why this works but thanks https://stackoverflow.com/questions/17862383/how-to-know-the-files-inside-the-tar-parser
            gzseek(file, skip, SEEK_CUR);
            n += skip + 512; //Next header
        }

        gzclose (file);
        return 0;
    }
}