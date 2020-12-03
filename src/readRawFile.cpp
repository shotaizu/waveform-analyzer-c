#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <getopt.h>
#include <algorithm>
#include <cctype>

int main(int argc, char **argv){

    double threshold = 1.;

    int c;
    int digit_optind = 0;

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"threshold",     required_argument, 0,  0 },
            {0,         0,                 0,  0 }
        };

        c = getopt_long(argc, argv, "",
                long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 0:
                switch(option_index){
                    case 0:
                        threshold = std::stod(optarg);
                        break;
                    default:
                        break;
                }
                break;
            case '?':
                break;
            default:
                break;
        }
    }

    //while (optind < argc)
    //    printf("%s ", argv[optind++]);

    std::vector<std::pair<double,double>> buffer;
    for(std::string line; std::getline(std::cin, line);){
        std::vector<std::string> fields;
        while(line.length() > 0){
            const size_t pos = line.find_first_of(',');
            if(pos == std::string::npos){
                fields.push_back(line);
                break;
            }
            fields.push_back( line.substr(0, pos));
            line = line.substr(pos+1, line.size() - pos);
        }

        if(fields.size() < 5) continue;
        std::cerr << fields[3] << " " << fields[4] << std::endl;
        std::transform(fields[3].begin(), fields[3].end(), fields[3].begin(), [](unsigned char c){return std::toupper(c);});
        std::transform(fields[4].begin(), fields[4].end(), fields[4].begin(), [](unsigned char c){return std::toupper(c);});

        double x,y;
        try{
            x = std::stod(fields[3]);
            y = std::stod(fields[4]);
        }catch(const std::invalid_argument& e){
            continue;
        }
        buffer.push_back(std::make_pair(x,y));
    }

    std::vector<double> trigtimes;
    if(buffer.size() < 2) return EXIT_SUCCESS;
    for(auto p = buffer.begin() + 1; p != buffer.end(); p++){
        const std::pair<double,double> now = *p;
        const std::pair<double,double> prev = *(p-1);
        if( now.second > threshold && prev.second < threshold)
            trigtimes.push_back( prev.first + (now.first - prev.first)/(now.second - prev.second)*(threshold - prev.second));
        std::cerr << now.first << " " << now.second << std::endl;
    }

    if(trigtimes.size() < 2) return EXIT_SUCCESS;
    for(auto t = trigtimes.begin()+1; t != trigtimes.end(); t++)
        std::cout << *t - *(t-1) << std::endl;

    return EXIT_SUCCESS;
}
