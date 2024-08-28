#ifndef UTILITY_H
#define UTILITY_H

#include <iostream>
#include <vector>
#include <filesystem>
#include <string>
#include <algorithm>


namespace utility{
    /** @brief get the file name from the dir path
     * @param path  the dir path
     * @return the file name of the path
     * 
    */
    std::vector<std::string> getFilesFromPath(const std::string& path);


    /** @brief get the file name from the dir path,but without sort algorithm
     * @param path  the dir path
     * @return the file name of the path
     * 
    */
    std::vector<std::string> getFilesFromPathWithoutSort(const std::string& path);




    
}






#endif