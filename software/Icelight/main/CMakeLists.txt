idf_component_register(SRCS "IceLight.cpp" "src/IceLED/IceLED.cpp" "src/IceLight_config.h" "src/IceLED/IceLED_Channel.cpp" "src/IceLED/lib8tion.h"
                        "src/IceLED/Colour_Utilities.cpp" "src/IceLED/Colour_Utilities.h" "src/IceLED/Colour_Utilities.h" "src/IceLED/hsv2rgb.cpp" 
                        "src/IceLED/Colour_Pallets.cpp" "src/IceServer/IceServer.h" "src/IceFS/IceFS.cpp"
                    INCLUDE_DIRS "."
                    "./src"
                    "./src/IceLED")


set(image ../fatfs_image)
#this will NOT flash the fat fs file system
fatfs_create_spiflash_image(storage ${image} PRESERVE_TIME)

#this WILL flash the fat fs file system and files.
#fatfs_create_spiflash_image(storage ${image} FLASH_IN_PROJECT PRESERVE_TIME)