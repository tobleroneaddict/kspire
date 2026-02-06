#pragma once
#include "globals.h"


namespace angel {

    //Reads a POSIX1003.1-1990 header

    //Individual asset pointer (obj, file, raw data, etc). Does not hold data, only points to a place in the tarball.
    struct Asset {
        char name[100]; //Name of Asset
        uint32_t length;  //Length (Without header)
        uint32_t offset;  //Location in file (after header)
    };

    /*
    *   Asset bundle format for nGL.
    *   TODO: Convert c style string usage to std::string.
    */
    struct Bundle {
        public:

        /*
        *   Load an asset bundle by name
        *   Usage: bundle.load_asset_bundle("level_0.tar.gz.tns")
        */
        int load_asset_bundle(const char* name);

        //List all files in the asset
        void debug_list_assets();

        /*
        *   Load raw data from a file.
        *   name: filename (including file extension)
        *   Usage: std::vector<uint8_t> credits = load_raw_data("credits.txt");
        */
        std::vector<uint8_t> load_raw_data(const char* name);

        //Free tar data from ram
        void free();


        private:

        bool compare(const char filename[100], const char* test);
        std::vector<Asset> assets;  //List of assets
        std::string bundle_name;    //Name of bundle, used for load_raw_data 
    };

}