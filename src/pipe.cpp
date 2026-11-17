#include "miniposix/pipe.hpp"

#include <cerrno>
#include <expected>
#include <fcntl.h>
#include <system_error>
#include <unistd.h>
#include <utility>

namespace miniposix {
namespace {

// Per AGENTS.md, recoverable OS errors travel as std::expected. Only the
// public create() converts an error into an exception, because the published
// header and its tests require that boundary behavior.
[[nodiscard]] std::expected<std::pair<UniqueFd, UniqueFd>, std::error_code> make_pipe_ends() noexcept {
    int ends[2] = {-1, -1};
    if (::pipe2(ends, O_CLOEXEC) == -1) {
        return std::unexpected(std::error_code(errno, std::generic_category()));
    }
    return std::pair<UniqueFd, UniqueFd>{UniqueFd{ends[0]}, UniqueFd{ends[1]}};
}

} // namespace

Pipe Pipe::create() {
    auto ends = make_pipe_ends();
    if (!ends) {
        throw std::system_error(ends.error(), "pipe2");
    }

    Pipe result;
    result.read_end_ = std::move(ends->first);
    result.write_end_ = std::move(ends->second);
    return result;
}

} // namespace miniposix
