#include <iostream>
#include <string>
#include <stdlib.h>
#include <TFile.h>
#include <TH1.h>
#include <getopt.h>


int main (int argc, char **argv){

    int c;
    int digit_optind = 0;
    std::pair<double, double> hist_ragnge { 0., 10e-9};
    int hist_nbins = 200;
    std::string ofname = "out.root";

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"xmin",     required_argument, 0,  0 },
            {"xmax",     required_argument, 0,  0 },
            {"nbins",     required_argument, 0,  0 },
            {0,         0,                 0,  0 }
        };

        c = getopt_long(argc, argv, "o:",
                long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 0:
                switch(option_index){
                    case 0:
                        hist_ragnge.first = std::stod(optarg);
                        break;
                    case 1:
                        hist_ragnge.second = std::stod(optarg);
                        break;
                    case 2:
                        hist_nbins = std::stoi(optarg);
                        break;
                    default:
                        break;
                }
                break;
            case 'o':
                ofname = std::string(optarg);
                break;
            case '?':
                break;
            default:
                break;
        }
    }

    //while (optind < argc)
    //    printf("%s ", argv[optind++]);

    TH1F *h = new TH1F("hist", ";;Entries / bin;", hist_nbins, hist_ragnge.first, hist_ragnge.second);
    h->Sumw2();

    for(std::string line; std::getline(std::cin, line);){
        double x;
        try{
            x = std::stod(line);
        } catch (const std::invalid_argument& e){
            continue;
        }
        h->Fill(x);
    }
    TFile *f = new TFile(ofname.c_str(), "RECREATE");
    h->Write();
    f->Save();
    f->Close();
    f->Delete();

    return EXIT_SUCCESS;
}
