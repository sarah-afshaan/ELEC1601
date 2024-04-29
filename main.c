#define SDL_MAIN_HANDLED
#include "stdio.h"
#include "stdlib.h"
#include "sdl.h"
#include "SDL2_gfxPrimitives.h"
#include "time.h"

#include "formulas.h"
#include "wall.h"
#include "robot.h"

int done = 0;


int main(int argc, char *argv[]) {
    SDL_SetMainReady();
    SDL_Window *window;
    SDL_Renderer *renderer;

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        return 1;
    }

    window = SDL_CreateWindow("Robot Maze", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 820, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, 0);

    struct Robot robot;
    struct Wall_collection *head = NULL;
    int front_centre_sensor, left_sensor, right_sensor=0;
    clock_t start_time, end_time;
    int msec;
    int crashed = 0;

    // SETUP MAZE
    // You can create your own maze here. line of code is adding a wall.
    // You describe position of top left corner of wall (x, y), then width and height going down/to right
    // Relative positions are used (OVERALL_WINDOW_WIDTH and OVERALL_WINDOW_HEIGHT)
    // But you can use absolute positions. 10 is used as the width, but you can change this.
    // Outer walls
    insertAndSetFirstWall(&head, 1, 100, 100, 800, 10); // Top wall
    insertAndSetFirstWall(&head, 2, 100, 100, 10, 500); // Left wall
    insertAndSetFirstWall(&head, 3, 100, 700, 800, 10); // Bottom wall
    insertAndSetFirstWall(&head, 4, 900, 100, 10, 500); // Right wall
    // Inner walls
    insertAndSetFirstWall(&head, 5, 200, 100, 10, 400); // Vertical wall 1
    insertAndSetFirstWall(&head, 6, 400, 500, 200, 10); // Horizontal wall 1
    insertAndSetFirstWall(&head, 7, 300, 200, 10, 300); // Vertical wall 2
    insertAndSetFirstWall(&head, 8, 300, 200, 100, 10); // Horizontal wall 2
    insertAndSetFirstWall(&head, 9, 400, 200, 10, 100); // Vertical wall 3
    insertAndSetFirstWall(&head, 10, 400, 300, 100, 10); // Horizontal wall 3
    insertAndSetFirstWall(&head, 11, 500, 200, 10, 400); // Vertical wall 4
    insertAndSetFirstWall(&head, 12, 500, 500, 100, 10); // Horizontal wall 4
    insertAndSetFirstWall(&head, 13, 600, 200, 10, 100); // Vertical wall 5
    insertAndSetFirstWall(&head, 14, 600, 300, 200, 10); // Horizontal wall 5
    insertAndSetFirstWall(&head, 15, 700, 100, 10, 300); // Vertical wall 6
    insertAndSetFirstWall(&head, 16, 600, 400, 110, 10); // Horizontal wall 6
    insertAndSetFirstWall(&head, 17, 700, 400, 10, 100); // Vertical wall 7
    insertAndSetFirstWall(&head, 18, 600, 600, 300, 10); // Horizontal wall 7
    insertAndSetFirstWall(&head, 19, 800, 200, 10, 300); // Vertical wall 8
    insertAndSetFirstWall(&head, 20, 400, 600, 10, 100); // Vertical wall 9
    insertAndSetFirstWall(&head, 21, 200, 600, 200, 10); // Horizontal wall 8
    insertAndSetFirstWall(&head, 22, 400, 500, 10, 100); // Vertical wall 10


    setup_robot(&robot);
    updateAllWalls(head, renderer);

    SDL_Event event;
    while(!done){
        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 50);
        SDL_RenderClear(renderer);

        //Move robot based on user input commands/auto commands
        if (robot.auto_mode == 1)
            robotAutoMotorMove(&robot, front_centre_sensor, left_sensor, right_sensor);
        robotMotorMove(&robot, crashed);

        //Check if robot reaches endpoint. and check sensor values
        if (checkRobotReachedEnd(&robot, 720, 700, 110, 10)){
            end_time = clock();
            msec = (end_time-start_time) * 1000 / CLOCKS_PER_SEC;
            robotSuccess(&robot, msec);
        }
        else if(crashed == 1 || checkRobotHitWalls(&robot, head)){
            robotCrash(&robot);
            crashed = 1;
        }
        //Otherwise compute sensor information
        else {
            front_centre_sensor = checkRobotSensorFrontCentreAllWalls(&robot, head);
            if (front_centre_sensor>0)
                printf("Getting close on the centre. Score = %d\n", front_centre_sensor);

            left_sensor = checkRobotSensorLeftAllWalls(&robot, head);
            if (left_sensor>0)
                printf("Getting close on the left. Score = %d\n", left_sensor);

            right_sensor = checkRobotSensorRightAllWalls(&robot, head);
            if (right_sensor>0)
                printf("Getting close on the right. Score = %d\n", right_sensor);
        }
        robotUpdate(renderer, &robot);
        updateAllWalls(head, renderer);

        // Check for user input
        SDL_RenderPresent(renderer);
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                done = 1;
            }
            const Uint8 *state = SDL_GetKeyboardState(NULL);
            if(state[SDL_SCANCODE_UP] && robot.direction != DOWN){
                robot.direction = UP;
            }
            if(state[SDL_SCANCODE_DOWN] && robot.direction != UP){
                robot.direction = DOWN;
            }
            if(state[SDL_SCANCODE_LEFT] && robot.direction != RIGHT){
                robot.direction = LEFT;
            }
            if(state[SDL_SCANCODE_RIGHT] && robot.direction != LEFT){
                robot.direction = RIGHT;
            }
            if(state[SDL_SCANCODE_SPACE]){
                setup_robot(&robot);
            }
            if(state[SDL_SCANCODE_RETURN]){
                robot.auto_mode = 1;
                start_time = clock();
            }
        }

        SDL_Delay(120);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    printf("DEAD\n");
}
