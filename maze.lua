local ffi = require('ffi')
local bit = require('bit')
local maze_lib = ffi.load('lib/libmaze.dylib')

ffi.cdef([[
typedef struct maze_t *maze_handle;

typedef enum maze_generation_t
{
	Sidewinder,
	NaiveSidewinder,
	RecursiveBacktrack,
	StackBacktrack,
	RecursiveDivision,
	StackDivision,
	EllersAlgorithm
} maze_generation_t;

typedef enum maze_cardinal_directions_t
{
	S = 1,
	E = 2,
	N = 4,
	W = 8
} maze_cardinal_directions_t;

maze_handle 
maze_create(const char *filepath, uint64_t width, uint64_t height);

maze_handle 
maze_open(const char *filepath);

void
maze_free(maze_handle maze);

uint8_t
maze_get(maze_handle maze, uint64_t x, uint64_t y);

void
maze_generate(maze_handle maze, maze_generation_t type);

uint64_t
maze_width(maze_handle maze);

uint64_t
maze_height(maze_handle maze);
]])

local MazeClass = {
	handle = nil
}
MazeClass.__index = MazeClass

local MazeGenerationType = {
	Sidewinder = ffi.C.Sidewinder
}

local MazeDirection = {
	South = ffi.C.S,
	East = ffi.C.E,
	North = ffi.C.N,
	West = ffi.C.W
}

function MazeClass.new(maze_path, width, height)
	local self = setmetatable({}, MazeClass)
	
	self.handle = maze_lib.maze_create(maze_path, width, height)

	ffi.gc(self.handle, maze_lib.maze_free)

	return self
end

function MazeClass.open(maze_path)
	local self = setmetatable({}, MazeClass)
	
	self.handle = maze_lib.maze_open(maze_path)

	ffi.gc(self.handle, maze_lib.maze_free)

	return self
end

function MazeClass:generate(type)
	maze_lib.maze_generate(self.handle, type)
end

function MazeClass:width()
	return tonumber(maze_lib.maze_width(self.handle))
end

function MazeClass:height()
	return tonumber(maze_lib.maze_height(self.handle))
end

function MazeClass:get(x, y)
	return maze_lib.maze_get(self.handle, x, y)
end

function MazeClass:test(x, y, direction)
	return bit.band(self:get(x, y), direction) > 0
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
