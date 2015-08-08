local maze_mod = require('maze')

-- local maze = maze_mod.MazeClass.new('/Users/bence/dev/maze_32768x32768.bin', 32768, 32768)
-- local maze = maze_mod.MazeClass.open('/Users/bence/dev/maze_32768x32768.bin')
local maze = maze_mod.MazeClass.open('/Users/bence/dev/maze_32x32.bin')
-- maze:generate(maze_mod.MazeGenerationType.Sidewinder)
local tile_size = 20
local zoom = 1
local maze_offset = {
	x = 0,
	y = 0
}

function love.keypressed(key, isrepeat)
	if key == 'escape' then
		love.event.push('quit')
	end
end

function love.load(arg)
	love.keyboard.setKeyRepeat(false)
	love.graphics.setBackgroundColor(255, 0, 0)
end

function love.update(dt)
	local shift_down = false
	local velocity = 1 / zoom

	if love.keyboard.isDown('lshift') or love.keyboard.isDown('rshift') then
		shift_down = true
	end

	if love.keyboard.isDown('up') then
		if shift_down then
			zoom = zoom + 0.05
		else
			maze_offset.y = maze_offset.y - velocity
		end
	elseif love.keyboard.isDown('down') then
		if shift_down then
			zoom = math.max(0.1, zoom - 0.05)
		else
			maze_offset.y = maze_offset.y + velocity
		end
	elseif love.keyboard.isDown('left') then
		maze_offset.x = maze_offset.x - velocity
	elseif love.keyboard.isDown('right') then
		maze_offset.x = maze_offset.x + velocity
	end
end

function love.draw()
	local sts = zoom * tile_size
	local scaled_first_tile = {
		x = maze_offset.x * sts,
		y = maze_offset.y * sts
	}
	local maze_in_view = {
		x = clamp(0 - maze_offset.x, 0, maze:width()),
		y = clamp(0 - maze_offset.y, 0, maze:height()),
		w = 0,
		h = 0
	}
	maze_in_view.w = clamp(love.graphics.getWidth() / sts - maze_offset.x, 0, maze:width())
	maze_in_view.h = clamp(love.graphics.getHeight() / sts - maze_offset.y, 0, maze:height())

	draw_maze_area(maze, {
		x = math.floor(maze_in_view.x),
		y = math.floor(maze_in_view.y),
		w = math.ceil(maze_in_view.w),
		h = math.ceil(maze_in_view.h)
	}, maze_offset)

	love.graphics.setColor(0, 255, 0)
	love.graphics.print(string.format('(%2.2f, %2.2f)', maze_offset.x, maze_offset.y), 0, 0)
end

function clamp(value, min, max)
	return math.min(max, math.max(min, value))
end

function draw_maze_area(maze, rect, offset)
	local sts = zoom * tile_size

	local total_drawn = 0

	for y = rect.y, rect.h - 1 do
		for x = rect.x, rect.w - 1 do
			local tile_point = {
				x = (offset.x + x) * sts,
				y = (offset.y + y) * sts
			}
			love.graphics.setColor(255, 255, 255)
			love.graphics.rectangle('fill', tile_point.x, tile_point.y, sts, sts)

			love.graphics.setColor(0, 0, 0)
			if not maze:test(x, y, maze_mod.MazeDirection.South) then
				love.graphics.line(
					tile_point.x, tile_point.y + sts,
					tile_point.x + sts, tile_point.y + sts
				)
			end

			if not maze:test(x, y, maze_mod.MazeDirection.East) then
				love.graphics.line(
					tile_point.x + sts, tile_point.y,
					tile_point.x + sts, tile_point.y + sts
				)
			end

			if y == 0 then
				love.graphics.line(tile_point.x, tile_point.y, tile_point.x + sts, tile_point.y)
			end

			if x == 0 then
				love.graphics.line(tile_point.x, tile_point.y, tile_point.x, tile_point.y + sts)
			end

			total_drawn = total_drawn + 1
		end
	end

	love.graphics.print(string.format('Total Drawn: %i', total_drawn), 0, 20)
end
