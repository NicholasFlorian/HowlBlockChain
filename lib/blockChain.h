#include <sstream>
#include <iostream>

#include "block.h"

namespace howl {

    class BlockChain {

    public:

        // constructor
        BlockChain();

        // mutators
        void addblock(char* message);
        char* toString();

    private:

        // variables
        uint32_t    _work;
        uint32_t    _length;
        Block*      _head;

        // get Last block
        Block       _getLastblock() const;
    };
}