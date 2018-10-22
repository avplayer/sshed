#pragma once

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>
#include <sstream>
#include <string>

class ssh_config
{
public:
    using ssh_conf_t = std::map<std::string,  std::string>;
    ssh_config()
    {}
    //  one map
    ssh_config(std::string &&host,
      std::string &&host_name,
      std::string &&port,
      std::string &&user,
      std::string &&identity_file):
      conf(std::vector<ssh_conf_t>({ssh_conf_t(
          {
                {"Host", std::move(host)},
                {"HostName", std::move(host_name)},
                {"Port", std::move(port)},
                {"User", std::move(user)},
                {"IdentityFile",  std::move(identity_file)},
          })}))
    {}
    //  many map
    ssh_config(std::initializer_list<ssh_conf_t> l): conf(l)
    {}
    //  a vector
    ssh_config(std::vector<ssh_conf_t> &&v): conf(std::move(v))
    {}
    // from file
    ssh_config(const std::filesystem::path &ssh_config_file)
    {
        std::ifstream config_in(ssh_config_file);
        std::string line, key, value, host_key, host_value;
        ssh_conf_t one_host;
        int add = true;
        while(std::getline(config_in, line))
        {
            std::istringstream record(line);
            record >> key;
            record >> value;
            if((key == "Host") && !one_host.empty())
            {
                conf.push_back(one_host);
                one_host.clear();
                one_host[key] = value;
            }
            else
                one_host[key] = value;
        }
    }

    // begin iterator
    std::vector<ssh_conf_t>::const_iterator begin()
    {
        return conf.cbegin();
    }
    // end iterator
    std::vector<ssh_conf_t>::const_iterator end()
    {
        return conf.cend();
    }

    std::vector<ssh_conf_t> value()
    {
        return conf;
    }

    bool exists(std::string& host)
    {
        for(auto &one_host : conf)
            if(host == one_host["Host"])
                return true;
        return false;
    }

    std::string auto_complete()
    {
        std::string auto_complete_str = "complete -W '";
        for(auto one_host : conf)
            auto_complete_str += one_host["Host"] + " ";
        auto_complete_str += "' ssh";
        std::cout << "the ssh auto complete script:\n"
                  << auto_complete_str << std::endl;
        return auto_complete_str;
    }

    int copy(std::filesystem::path src = "~/.ssh/config",
             std::filesystem::path trg = "~/.ssh/config.bk")
    {
        try
        {
            std::filesystem::copy_file(src, trg, std::filesystem::copy_options::update_existing);
        }
        catch(std::filesystem::filesystem_error& e)
        {
            std::cout << e.what() << '\n';
        }
        return 0;
    }

    int list(bool simple = true)
    {
        auto conf_sort = conf;
        std::stable_sort(conf_sort.begin(),  conf_sort.end());
        std::cout << "ssh config have " <<  conf_sort.size() <<  "items\n";
        if(simple)
            std::cout << std::left << std::setw(20) << "Host"
                  << std::right << std::setw(20) << "HostName"
                  << std::endl;
        for(auto &one_host : conf_sort)
        {
            if(!simple)
            {
                for(auto &item : one_host)
                    std::cout << item.first << ":" << item.second << "\t";
                std::cout << "\n";
            }
            else
            {
                std::cout << std::left << std::setw(20) << one_host["Host"];
                auto search = one_host.find("HostName");
                std::string hostname;
                if(search != one_host.end())
                    hostname = search->second;
                auto search_ = one_host.find("Port");
                if(search_ != one_host.end())
                    hostname = hostname + ":" + search_->second;
                if(hostname != "")
                    std::cout << std::right << std::setw(20) << hostname;
                std::cout << "\n";
            }
        }
        return 0;
    }

    int add()
    {
        bool exists;
        ssh_conf_t host_conf;
        std::cout << "Host:\n";
        while(std::cin >> host_conf["Host"])
        {
            exists = false;
            for(auto one_host : conf)
                if(one_host["Host"] == host_conf["Host"])
                {
                    exists = true;
                    std::cout << host_conf["Host"] << " exists!\n";
                    std::cout << "current info:\n";
                    for(auto& item : one_host)
                        std::cout << item.first << ": " << item.second << "\t";

                    std::cout << "Override it?(yes|y)" <<  std::endl;
                    std::string yes;
                    std::cin >> yes;
                    if(yes == "yes" || yes == "y")
                        exists = false;
                    break;
                }

            if(!exists)
            {
                std::cout << "HostName:\n";
                std::cin >> host_conf["HostName"];
                std::cout << "Port:\n";
                std::cin >> host_conf["Port"];
                std::cout << "User:\n";
                std::cin >> host_conf["User"];
                std::cout << "IdentityFile:\n";
                std::cin >> host_conf["IdentityFile"];
                conf.push_back(host_conf);
            }
            else
                std::cout << "Host:\n";
        }
        return 0;
    }

    int del()
    {
        bool exists;
        std::cout << "Please input a host to delete:\n";
        std::string host;
        while(std::cin >> host)
        {
            exists = false;
            for(auto i = conf.begin(); i != conf.end(); ++i)
                if(i->at("Host") == host)
                {
                    exists = true;
                    conf.erase(i);
                    std::cout << host << " has been deleted\n"
                              << "Please input a host to delete (ctrl-c to cancel):\n";
                    break;
                }

            if(!exists)
                std::cout << host << " is not exists\n"
                          << "Please input a exists host to delete:\n";
        }
        return 0;
    }

    ssh_config& operator+=(ssh_config& rhs)
    {
        for(auto one_host : rhs)
            if(exists(one_host["Host"]))
                std::cout << "Host " << one_host["Host"] << " is exit, ingore this items, you have to change or delete old item and readd it.\n";
            else
            {
                std::cout << "add host: " << one_host["Host"] << std::endl;
                conf.push_back(one_host);
            }
        return *this;
    }

    ssh_config& operator+=(const std::filesystem::path& file)
    {
        auto rhs = ssh_config(file);
        *this += rhs;
        return *this;
    }

    ssh_config& operator-=(ssh_config& rhs)
    {
        std::vector<decltype(conf.begin())> del_v;
        for(auto i = conf.begin(); i != conf.end(); ++i)
            for(auto one_host : rhs)
            {
                if(i->at("Host") == one_host["Host"])
                {
                    std::cout << "remove host: " << one_host["Host"] << std::endl;
                    del_v.push_back(i);
                    break;
                }
                else
                    std::cout << "host " << one_host["Host"] << " is not exists\n";
            }

        // conf.erase(*del_v.begin(),  *del_v.end());
        for(auto i : del_v)
            conf.erase(i);
        return *this;
    }

    ssh_config& operator-=(const std::filesystem::path& file)
    {
        auto rhs = ssh_config(file);
        *this -= rhs;
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os,  ssh_config&);

    int write()
    {
        std::ofstream config_file("~/.ssh/config", std::ofstream::out);
        if(config_file)
            config_file << *this;
        return 0;
    }

private:
    std::vector<ssh_conf_t> conf;
};

// print or write function
std::ostream& operator<<(std::ostream &os,  ssh_config &conf)
{
    for(auto one_host : conf)
    {
        for(auto item : one_host)
            if(item.first ==  "Host")
                os << item.first << " " << item.second << "\n";
            else
                os << "\t" << item.first << " " << item.second << "\n";
    }
    os << std::endl;
    return os;
}