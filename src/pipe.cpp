#include "miniposix/pipe.hpp"

#include <cerrno>
#include <fcntl.h>
#include <system_error>
#include <unistd.h>

namespace miniposix {

Pipe Pipe::create() {
    int ends[2] = {-1, -1};
    if (::pipe2(ends, O_CLOEXEC) == -1) {
        throw std::system_error(errno, std::generic_category(), "pipe2");
    }

    Pipe result;
    result.read_end_ = UniqueFd{ends[0]};
    result.write_end_ = UniqueFd{ends[1]};
    return result;
}

} // namespace miniposix
