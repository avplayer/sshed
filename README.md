`bash
mkdir build
cd build
cmake ..
make -j8
`

`bash
./bin/sshed --help
AME:
        sshed-c - SSH config editor and hosts manager
USAGE:
        sshed-c command [arguments...]
VERSION:
        0.0.1(beta)
AUTHOR:
        yirenyiai<lushuwen_gz@formail.com>
COMMANDS
        show    Shows host
        list    Lists all hosts
        auto_complete   Set auto complete of all hosts
        add     Add hosts
        adds    Add hosts from config file, the config file line format
                eg:Host:<host>,Port:<port>,User:<user>,IdentityFile:<identityfile>
        remove  Removes hosts, removes all when ssh host unname pass
        rollback        roll back last operator
        clean_cache     clean ssh cache
        help    this message
`

`bash
./bin/sshed.cc --help
AME:
        sshed - SSH config editor and hosts manager
USAGE:
        sshed.cc command [arguments...]
VERSION:
        0.0.1(beta)
AUTHOR:
        yirenyiai<lushuwen_gz@formail.com>
COMMANDS
        show                  Shows host
        list                  Lists all hosts
        auto_complete         Set auto complete of all hosts
        backup                backup current ssh config
        rollback              Roll back last operator
        add                   Add hosts
        adds                  Add hosts from config file (default ssh config file)
        del                   Del hosts
        dels                  Del hosts from config file (default ssh config file)
        help | h              This message
`