#include "obj/player.hh"

namespace glp {

namespace Object {

void PlayerFPP::mouse_update(int x, int y) {
    if(firstmouse) {
        x = camera->get_dimensions().x/2,
          y = camera->get_dimensions().y/2;
        firstmouse = false;
    }
    rot_off.x = x;
    rot_off.y = y;

    float sens = 0.1f;
    rot_off.x*=sens;
    rot_off.y*=sens;
    camera->yaw_change(rot_off.x);
    camera->pitch_change(-rot_off.y);
    camera->calculate();
}

void PlayerFPP::fpp_movement(float dt) {
    camera->position_change((move_speed*dt*camera->get_front())*move_dir.y);
    camera->position_change((glm::normalize(glm::cross(camera->get_front(), camera->get_up()))*move_speed*dt)*move_dir.x);
    if(!mouse) {
        camera->yaw_change(rot_off.x);
        camera->pitch_change(-rot_off.y);
        camera->calculate();
    }
}

void PlayerFPP::fpp_movement_keys() {
    for(auto& event: *events) {
        if(event.type == SDL_KEYDOWN) {
            if(event.key.keysym.sym == SDLK_w) move_dir.y = 1.0f;
            if(event.key.keysym.sym == SDLK_s) move_dir.y = -1.0f;
            if(event.key.keysym.sym == SDLK_a) move_dir.x = -1.0f;
            if(event.key.keysym.sym == SDLK_d) move_dir.x = 1.0f;
        } else if(event.type == SDL_KEYUP) {
            if(event.key.keysym.sym == SDLK_w) move_dir.y = 0.0f;
            if(event.key.keysym.sym == SDLK_s) move_dir.y = 0.0f;
            if(event.key.keysym.sym == SDLK_a) move_dir.x = 0.0f;
            if(event.key.keysym.sym == SDLK_d) move_dir.x = 0.0f;
        } else if(event.type == SDL_MOUSEMOTION) {
            mouse_update(event.motion.xrel, event.motion.yrel);
        } else if(event.type == SDL_CONTROLLERAXISMOTION) {
            if(event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTX) {
                if(event.caxis.value < -8000) move_dir.x = -1.0f;
                else if(event.caxis.value > 8000) move_dir.x = 1.0f;
                else if(event.caxis.value > -8000 && event.caxis.value < 8000) move_dir.x = 0.0f;
            }
            if(event.caxis.axis == SDL_CONTROLLER_AXIS_LEFTY) {
                if(event.caxis.value < -8000) move_dir.y = 1.0f;
                else if(event.caxis.value > 8000) move_dir.y = -1.0f;
                else if(event.caxis.value > -8000 && event.caxis.value < 8000) move_dir.y = 0.0f;
            }
            if(event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTX) {
                if(event.caxis.value < -8000) rot_off.x = -1.0f;
                else if(event.caxis.value > 8000) rot_off.x = 1.0f;
                else if(event.caxis.value > -8000 && event.caxis.value < 8000) rot_off.x = 0.0f;
            }
            if(event.caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY) {
                if(event.caxis.value < -8000) rot_off.y = -1.0f;
                else if(event.caxis.value > 8000) rot_off.y = 1.0f;
                else if(event.caxis.value > -8000 && event.caxis.value < 8000) rot_off.y = 0.0f;
            }
        }
    }
}

PlayerCollFPP::PlayerCollFPP(float speed, Camera* cam, std::vector<SDL_Event>* sdl) : PlayerFPP{speed, cam, sdl}, Collidable{new btBoxShape(btVector3(1,2,1)), 1.0f, btVector3(cam->get_position().x, cam->get_position().y, cam->get_position().z)} {
    
}

void PlayerCollFPP::update() {
    rigidbody->activate();
    btTransform t;
    motion_state->getWorldTransform(t);
    auto origin = t.getOrigin();
    camera->set_position(glm::vec3(origin.getX()+cam_off.x,
                origin.getY()+cam_off.y,
                origin.getZ()+cam_off.z));
    auto forward = glm::mat3(glm::inverse(camera->view()))*glm::vec3(move_dir.x,0,-move_dir.y);
    float moving = (move_dir.x != 0.0f ? 1.0f : 0.0f) || (move_dir.y != 0.0f ? 1.0f : 0.0f);
    rigidbody->setLinearVelocity(btVector3(moving*move_speed*forward.x,
                rigidbody->getGravity().getY(),
                moving*move_speed*forward.z));
    if(!mouse) {
        camera->yaw_change(rot_off.x);
        camera->pitch_change(-rot_off.y);
        camera->calculate();
    }
}

}

}
