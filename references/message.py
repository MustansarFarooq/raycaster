# Example file showing a circle moving on screen
import math

import pygame

# pygame setup
pygame.init()
screen = pygame.display.set_mode((1280, 720), pygame.RESIZABLE)
pygame.display.set_caption("Raycaster")
clock = pygame.time.Clock()
running = True
dt = 0

fov = 90
resolution = screen.get_width()/1

level = [
    [1, 1, 1, 1, 1, 1, 1, 1],
    [1, 0, 1, 0, 0, 0, 0, 1],
    [1, 0, 1, 0, 0, 0, 0, 1],
    [1, 0, 1, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 1],
    [1, 0, 0, 0, 0, 1, 0, 1],
    [1, 0, 0, 0, 0, 0, 0, 1],
    [1, 1, 1, 1, 1, 1, 1, 1]
]

player_pos = pygame.Vector2(8 / 2, 8 / 2)
player_direction = 0
player_speed = 2
cellSize = 50


def clamp(n, min_value, max_value):
    return max(min_value, min(n, max_value))


def distance(pos1: pygame.Vector2, pos2: pygame.Vector2):
    return (pos1 - pos2).magnitude()


def raycast():
    ray_x, ray_y, ray_angle, x_offset, y_offset = 0, 0, 0, 0, 0
    ray_angle = player_direction - fov / 2
    ray_angle = ray_angle % 360
    lineWidth = screen.get_width() / resolution
    for x in range(math.floor(resolution)):

        final_distance = 100000000000000000

        # check horizontal lines

        horizontal_pos, horizontal_distance = pygame.Vector2(), 1000000000

        steps, depth_of_field = 0, 8
        if math.tan(    (ray_angle)) == 0:
            atan = -1 / 0.00000000000000000001
        else:
            atan = -1 / math.tan(math.radians(ray_angle))
        if ray_angle > 180:  # looking up
            ray_y = math.floor(player_pos.y)
            ray_x = (player_pos.y - ray_y) * atan + player_pos.x
            y_offset = -1
            x_offset = - y_offset * atan
        if ray_angle < 180:  # looking down
            ray_y = math.ceil(player_pos.y)
            ray_x = (player_pos.y - ray_y) * atan + player_pos.x
            y_offset = 1
            x_offset = - y_offset * atan

        if ray_angle == 0 or ray_angle == 180:
            steps = depth_of_field
        while steps < depth_of_field:
            cell_x, cell_y = int(ray_x), int(ray_y)
            if (0 <= cell_y < len(level) and 0 <= cell_x < len(level[cell_y]) and
                    (level[cell_y][cell_x] == 1 or (cell_y > 0 and level[cell_y - 1][cell_x] == 1))):
                steps = depth_of_field
                horizontal_pos.x = ray_x
                horizontal_pos.y = ray_y

                horizontal_distance = distance(player_pos, horizontal_pos)
            else:
                ray_x += x_offset
                ray_y += y_offset
                steps += 1
        #     pygame.draw.circle(screen, pygame.Color("#3F4739"), pygame.Vector2(ray_x, ray_y) * cellSize, 5)
        # pygame.draw.line(screen, pygame.Color(0, 255, 0), player_pos * cellSize, pygame.Vector2(ray_x * cellSize, ray_y * cellSize), 8)

        # check vertical lines

        vertical_pos, vertical_distance = pygame.Vector2(), 1000000000

        steps, depth_of_field = 0, 8

        atan = -math.tan(math.radians(ray_angle))

        if 90 < ray_angle < 270:  # looking left
            ray_x = math.floor(player_pos.x)
            ray_y = (player_pos.x - ray_x) * atan + player_pos.y
            x_offset = -1
            y_offset = - x_offset * atan
        if ray_angle < 90 or ray_angle > 270:  # looking right
            ray_x = math.ceil(player_pos.x)
            ray_y = (player_pos.x - ray_x) * atan + player_pos.y
            x_offset = 1
            y_offset = - x_offset * atan

        if ray_angle == 270 or ray_angle == 90:
            steps = depth_of_field
        while steps < depth_of_field:
            cell_x, cell_y = int(ray_x), int(ray_y)
            if (0 <= cell_y < len(level) and 0 <= cell_x < len(level[cell_y]) and
                    (level[cell_y][cell_x] == 1 or (cell_x > 0 and level[cell_y][cell_x - 1] == 1))):
                steps = depth_of_field
                vertical_pos.x = ray_x
                vertical_pos.y = ray_y

                vertical_distance = distance(player_pos, vertical_pos)
            else:
                ray_x += x_offset
                ray_y += y_offset
                steps += 1
        wallColor = pygame.Color(0, 255, 0)
        if horizontal_distance < vertical_distance:
            pygame.draw.line(screen, pygame.Color(255, 0, 0), player_pos * cellSize, horizontal_pos * cellSize, 3)
            final_distance = horizontal_distance
            wallColor = pygame.Color(255, 0, 0)
        if vertical_distance < horizontal_distance:
            pygame.draw.line(screen, pygame.Color(255, 0, 0), player_pos * cellSize, vertical_pos * cellSize, 3)
            final_distance = vertical_distance
            wallColor = pygame.Color(128, 0, 0)
        ray_angle_difference = (player_direction - ray_angle) % 360
        final_distance = final_distance * math.cos(math.radians(ray_angle_difference))
        if final_distance == 0:
            final_distance = 0.0000001
        lineHeight = (screen.get_height() / final_distance * 0.5)
        lineStep = x / resolution
        pygame.draw.line(screen, wallColor, pygame.Vector2(lineStep * screen.get_width() + math.ceil(lineWidth)/2, (screen.get_height() / 2)),
                         pygame.Vector2(lineStep * screen.get_width() + math.ceil(lineWidth)/2, (screen.get_height() / 2) + lineHeight),
                         math.ceil(screen.get_width() / resolution))
        pygame.draw.line(screen, wallColor,
                         pygame.Vector2((lineStep) * screen.get_width() + math.ceil(lineWidth)/2, (screen.get_height() / 2)),
                         pygame.Vector2((lineStep) * screen.get_width() + math.ceil(lineWidth)/2, (screen.get_height() / 2) - lineHeight),
                         math.ceil(lineWidth))
        ray_angle += (fov / resolution)
        ray_angle = ray_angle % 360

    # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    # # my own code </3
    # tangent = math.tan(math.radians(player_direction))
    # if player_direction > 180:  # looking up
    #     ray_difference_y = math.floor(player_pos.y) - player_pos.y
    # else:  # looking down
    #     ray_difference_y = math.ceil(player_pos.y) - player_pos.y
    # if tangent == 0:
    #     return
    # ray_difference_x = ray_difference_y / tangent
    # pygame.draw.circle(screen, pygame.Color("#3F4739"),
    #                    (player_pos + pygame.Vector2(ray_difference_x, ray_difference_y)) * cellSize, 5)


def angle_to_vector(angle):
    return pygame.Vector2(math.cos(math.radians(angle)), math.sin(math.radians(angle)))


def draw_level():
    for y in range(len(level)):
        for x in range(len(level[y])):
            if level[y][x]:
                pygame.draw.rect(screen, pygame.Color("#BACBA9"),
                                 pygame.Rect(x * cellSize, y * cellSize, cellSize, cellSize))
            pygame.draw.rect(screen, pygame.Color("#717568"),
                             pygame.Rect(x * cellSize, y * cellSize, cellSize, cellSize), 1)


while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    screen.fill(pygame.Color("#E1F4CB"))
    draw_level()
    raycast()
    pygame.draw.circle(screen, pygame.Color("#3F4739"), player_pos * cellSize, 5)
    # pygame.draw.line(screen, pygame.Color("#3F4739"), player_pos * cellSize,
    #                  (player_pos * cellSize) + angle_to_vector(player_direction) * cellSize, 3)
    keys = pygame.key.get_pressed()
    if keys[pygame.K_w]:
        player_pos += angle_to_vector(player_direction) * player_speed * dt
    if keys[pygame.K_s]:
        player_pos -= angle_to_vector(player_direction) * player_speed * dt
    if keys[pygame.K_a]:
        player_pos += angle_to_vector(player_direction - 90) * player_speed * dt
    if keys[pygame.K_d]:
        player_pos += angle_to_vector(player_direction + 90) * player_speed * dt
    if keys[pygame.K_LEFT]:
        player_direction -= 180 * dt
    if keys[pygame.K_RIGHT]:
        player_direction += 180 * dt

    player_direction = player_direction % 360
    pygame.display.flip()

    dt = clock.tick(120) / 1000

pygame.quit()
