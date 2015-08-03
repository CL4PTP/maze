local ffi = require('ffi')
local maze_lib = ffi.load('lib/libmaze.dylib')

ffi.cdef([[
typedef struct maze_raw_t
{
	uint64_t width;
	uint64_t height;
	uint32_t arr[];
} maze_raw_t;

typedef struct maze_t
{
	int fd;
	maze_raw_t *raw;
} maze_t;

typedef enum maze_generation_t
{
	Sidewinder,
	NaiveSidewinder
} maze_generation_t;

typedef enum maze_cardinal_directions_t
{
	S = 1,
	E = 2,
	N = 4,
	W = 8
} maze_cardinal_directions_t;

maze_t *
maze_create(const char *filename, uint64_t width, uint64_t height);

maze_t *
maze_open(const char *filepath);

void
maze_free(maze_t *maze);

uint8_t
maze_get(maze_t *maze, uint64_t x, uint64_t y);

void
maze_generate(maze_t *maze, maze_generation_t type);

bool
maze_test(maze_t *maze, uint64_t x, uint64_t y, maze_cardinal_directions_t direction);
]])

local MazeClass = {
	maze_raw = nil
}
MazeClass.__index = MazeClass

local MazeGenerationType = {
	Sidewinder = ffi.C.Sidewinder,
	NaiveSidewinder = ffi.C.NaiveSidewinder
}

local MazeDirection = {
	South = ffi.C.S,
	East = ffi.C.E
}

function MazeClass.new(maze_path, width, height)
	local self = setmetatable({}, MazeClass)
	
	self.maze_raw = maze_lib.maze_create(maze_path, width, height)

	ffi.gc(self.maze_raw, maze_lib.maze_free)

	return self
end

function MazeClass.open(maze_path)
	local self = setmetatable({}, MazeClass)
	
	self.maze_raw = maze_lib.maze_open(maze_path)

	ffi.gc(self.maze_raw, maze_lib.maze_free)

	return self
end

function MazeClass:generate(type)
	maze_lib.maze_generate(self.maze_raw, type)
end

function MazeClass:width()
	return tonumber(self.maze_raw.raw.width)
end

function MazeClass:height()
	return tonumber(self.maze_raw.raw.height)
end

function MazeClass:get(x, y)
	return maze_lib.maze_get(self.maze_raw, x, y)
end

function MazeClass:test(x, y, direction)
	return maze_lib.maze_test(self.maze_raw, x, y, direction)
end

function MazeClass:repr()
	local buf = ""

	buf = buf .. " " .. string.rep('_', self:width() * 2 - 1) .. "\n"

	for y = 0, self:height() - 1 do
		buf = buf .. '|'

		for x = 0, self:width() - 1 do
			if self:test(x, y, MazeDirection.South) then
				buf = buf .. ' '
			else
				buf = buf .. '_'
			end

			if self:test(x, y, MazeDirection.East) then
				buf = buf .. '.'
			else
				buf = buf .. '|'
			end
		end

		buf = buf .. "\n"
	end

	return buf
end

return {
	MazeClass = MazeClass,
	MazeGenerationType = MazeGenerationType,
	MazeDirection = MazeDirection
}
