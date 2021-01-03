#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <getopt.h>
#include <algorithm>
#include <iomanip>
#include <cctype>

void showHelp(const char *arg0){
    std::cout << arg0 << " {[raw-csv_file]} {options}\n\n"
        << "Usage:\n"
        << "[raw-csv_file]: waveform-file of oscilloscope (should be csv format of Tektronix for now)\n\n"
        << "OPTIONS: -h, --help, --threshold, --mestype, --trigger, --period, --autothreshold\n"
        << "\t-h, --help: show this help\n"
        << "\t--mestype {trigger, period}: select measurement type: trigger=relative timing, period=calculate period\n"
        << "\t--trigger: equivalent to \"--mestype trigger\"\n"
        << "\t--period: equiavalent to \"--mestype period\" (default)\n"
        << "\t--autothreshold: set threshold to 50\% of amplitude\n"
        << "\n"
        << "Copyright 2020 Shota Izumiyama" << std::endl;
    return;
}

int main(int argc, char **argv){
    enum MESTYPE {kMesTypePeriod, kMesTypeTrigger};
    enum THRTYPE {kThrFixed, kThrAuto};

    double threshold = 1.;
    THRTYPE thr_type = kThrFixed;
    double trig_time = 0.;
    std::vector<std::string> fnames;

    int c;
    int digit_optind = 0;

    MESTYPE mes_type = kMesTypePeriod;

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"threshold",     required_argument, 0,  0 },
            {"mestype",     required_argument, 0,  0 },
            {"trigger",     no_argument, 0,  0 },
            {"period",     no_argument, 0,  0 },
            {"help",     no_argument, 0,  0 },
            {"autothreshld", no_argument, 0, 0},
            {0,         0,                 0,  0 }
        };

        c = getopt_long(argc, argv, "h",
                long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 0:
                switch(option_index){
                    case 0:
                        threshold = std::stod(optarg);
                        break;
                    case 1:
                        if( std::string{"trigger"} == std::string{optarg})
                            mes_type = kMesTypeTrigger;
                        else if( std::string{"period"} == std::string{optarg})
                            mes_type = kMesTypePeriod;
                        else {
                            std::cerr << "Could not understand measurement type = " << optarg
                                << "\nPlease select \"trigger\" or \"period\"" << std::endl;
                            return EXIT_FAILURE;
                        }
                        break;
                    case 2:
                        mes_type = kMesTypeTrigger;
                        break;
                    case 3:
                        mes_type = kMesTypePeriod;
                        break;
                    case 4:
                        showHelp(argv[0]);
                        return EXIT_SUCCESS;
                        break;
                    case 5:
                        thr_type = kThrAuto;
                        break;
                    default:
                        break;
                }
                break;
            case 'h':
                showHelp(argv[0]);
                return EXIT_SUCCESS;
                break;
            case '?':
                break;
            default:
                break;
        }
    }

    while (optind < argc)
        fnames.push_back(std::string{argv[optind++]});


    std::istream *istrm;
    std::ifstream ifstrm;
    if(fnames.size()>0){
        ifstrm.open(fnames[0]);
        if(!ifstrm.is_open()){
            std::cerr << "Failed to open : " << fnames[0] << std::endl;
            return EXIT_FAILURE;
        }
        istrm = &ifstrm;
    }
    else
        istrm = &std::cin;




    std::vector<std::pair<double,double>> buffer;
    for(std::string line; std::getline(*istrm, line);){
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

        if(mes_type == kMesTypeTrigger && fields[0] == std::string{"\"Trigger Time\""}){
            try{
                trig_time = std::stod(fields[1]);
            } catch (const std::invalid_argument& e){
                trig_time = 0.;
                std::cerr << "Failed to read trigger time... Will use t=0. as trigger time instead..." << std::endl;
            }
            //std::cerr << "Found Trigger Time!!!" << trig_time << std::endl;
        }

        double x,y;
        try{
            x = std::stod(fields[3]);
            y = std::stod(fields[4]);
        }catch(const std::invalid_argument& e){
            continue;
        }
        buffer.push_back(std::make_pair(x,y));
    }

    if(ifstrm.is_open()) ifstrm.close();

    if(thr_type == kThrAuto){
        //std::pair<std::pair<double,double>, std::pair<double,double>> minmax = std::minmax_element(buffer.begin(), buffer.end(), [](const std::pair<double,double> &a, const std::pair<double,double> &b){
        const auto minmax = std::minmax_element(buffer.begin(), buffer.end(), [](const std::pair<double,double> &a, const std::pair<double,double> &b){
                return a.second < b.second;});
        threshold = (minmax.second->second + minmax.first->second)/2.;
        std::cerr << "Auto threshold = " << threshold << std::endl;
    }

    std::vector<double> trigtimes;
    if(buffer.size() < 2) return EXIT_SUCCESS;
    for(auto p = buffer.begin() + 1; p != buffer.end(); p++){
        const std::pair<double,double> now = *p;
        const std::pair<double,double> prev = *(p-1);
        if( now.second >= threshold && prev.second < threshold)
            trigtimes.push_back( prev.first + (now.first - prev.first)/(now.second - prev.second)*(threshold - prev.second));
    }

    switch (mes_type){
        case kMesTypePeriod:
            if(trigtimes.size() < 2) return EXIT_SUCCESS;
            for(auto t = trigtimes.begin()+1; t != trigtimes.end(); t++){
                if( *t < *(t-1) ) continue;
                std::cout << std::left << std::setw(15) << *t - *(t-1) << std::endl;
            }
            break;
        case kMesTypeTrigger:
            for(auto t = trigtimes.begin(); t != trigtimes.end(); t++){
                if( *t > trig_time){
                    std::cout << std::left << std::setw(15) << *t - trig_time << std::endl;
                    break;
                }
            }
            break;
        default:
            break;
    }

    return EXIT_SUCCESS;
}
