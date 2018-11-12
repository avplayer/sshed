#include <sshed.hpp>

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <string>

void usage()
{
    std::cout << "AME:\n"
              << "\tsshed - SSH config editor and hosts manager\r\n"
              << "USAGE:\n"
              << "\tsshed.cc command [arguments...]\r\n"
              << "VERSION:\n"
              << "\t0.0.1(beta)\r\n"
              << "AUTHOR:\n"
              << "\tyirenyiai<lushuwen_gz@formail.com>\r\n"

              << "COMMANDS\n"
              << std::left << std::setw(20) << "\tshow\t" << "Shows host\n"
              << std::left << std::setw(20) << "\tlist\t" << "Lists all hosts\n"
              << std::left << std::setw(21) << "\tauto_complete\t" << "Set auto complete of all hosts\n"
              << std::left << std::setw(21) << "\tbackup\t" << " backup current ssh config\n"
              << std::left << std::setw(16) << "\trollback\t" << "Roll back last operator\n"

              << std::left << std::setw(19) << "\tadd\t" << "Add hosts\n"
              << std::left << std::setw(20) << "\tadds\t" << "Add hosts from config file (default ssh config file)\n"
              << std::left << std::setw(19) << "\tdel\t" << "Del hosts\n"
              << std::left << std::setw(20) << "\tdels\t" << "Del hosts from config file (default ssh config file)\n"

              << std::left << std::setw(16) << "\thelp | h\t" << "This message\n"
              << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        usage();
        return -1;
    }
    else
    {
        // std::filesystem::path home = std::getenv("HOME");
        // auto conf_file = home / ".ssh/config";
        // auto config = ssh_config(conf_file);
        auto config = ssh_config("~/.ssh/config");
        std::string options = argv[1];

        if(options == "h" || options == "help" ||
           options ==  "-h" || options == "--help")
            usage();
        else if(options == "list")
            config.list();
        else if(options == "show")
            std::cout << config;
        else if(options == "auto_complete")
            config.auto_complete();
        else if(options == "backup")
            config.copy();
        else if(options == "rollback")
            config.copy("~/.ssh/config.bk",  "~/.ssh/config");
        else if(options == "add")
        {
            config.copy();
            config.add();
            config.write();
        }
        else if(options == "adds")
        {
            if(argc > 2)
            {
                config.copy();
                config += argv[2];
                config.write();
            }
            else
                std::cout << "adds param miss. you must call is like: sshed.cc adds [file]\n";
        }
        else if(options == "del")
        {
            config.copy();
            config.del();
            config.write();
        }
        else if(options == "dels")
        {
            if(argc > 2)
            {
                config.copy();
                config -= argv[2];
                config.write();
            }
            else
                std::cout << "dels param miss. you must call is like: sshed .cc dels [file]\n";
        }

        return 0;
    }
}