/*
 * Copyright 2011 Nate Koenig & Andrew Howard
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#include <sstream>

#include "msgs/msgs.h"
#include "gui/GuiEvents.hh"
#include "common/MouseEvent.hh"
#include "math/Quaternion.hh"

#include "rendering/UserCamera.hh"

#include "transport/Publisher.hh"

#include "gui/SphereMaker.hh"

using namespace gazebo;
using namespace gui;

unsigned int SphereMaker::counter = 0;

SphereMaker::SphereMaker() 
  : EntityMaker()
{
  this->state = 0;
  this->visualMsg = new msgs::Visual();
  this->visualMsg->set_render_type( msgs::Visual::MESH_RESOURCE );
  this->visualMsg->set_mesh_type( msgs::Visual::SPHERE );
  this->visualMsg->set_material_script( "Gazebo/TurquoiseGlowOutline" );
  msgs::Set(this->visualMsg->mutable_pose()->mutable_orientation(), math::Quaternion());
}

SphereMaker::~SphereMaker()
{
  delete this->visualMsg;
}

void SphereMaker::Start(const rendering::UserCameraPtr camera)
                        //const CreateCallback &cb)
{
  //this->createCB = cb;
  this->camera = camera;
  std::ostringstream stream;
  stream << "user_sphere_" << counter++;
  this->visualMsg->mutable_header()->set_str_id( stream.str() );

  this->state = 1;
}

void SphereMaker::Stop()
{
  this->visualMsg->set_action( msgs::Visual::DELETE );
  this->visPub->Publish(*this->visualMsg);
  this->visualMsg->set_action( msgs::Visual::UPDATE );

  gui::Events::moveModeSignal(true);
  this->state = 0;
}

bool SphereMaker::IsActive() const
{
  return this->state > 0;
}

void SphereMaker::OnMousePush(const common::MouseEvent &event)
{
  if (this->state == 0)
    return;

  this->mousePushPos = event.pressPos;
}

void SphereMaker::OnMouseRelease(const common::MouseEvent &/*_event*/)
{
  if (this->state == 0)
    return;

  this->state++;

  if (this->state == 2)
  {
    this->CreateTheEntity();
    this->Stop();
  }
}

void SphereMaker::OnMouseDrag(const common::MouseEvent &event)
{
  if (this->state == 0)
    return;

  math::Vector3 norm;
  math::Vector3 p1, p2;

  norm.Set(0,0,1);

  p1 = this->camera->GetWorldPointOnPlane(this->mousePushPos.x, this->mousePushPos.y, norm, 0);
  p1 = this->GetSnappedPoint( p1 );

  p2 = this->camera->GetWorldPointOnPlane(event.pos.x, event.pos.y ,norm, 0);
  p2 = this->GetSnappedPoint( p2 );

  msgs::Set(this->visualMsg->mutable_pose()->mutable_position(), p1);

  double scale = p1.Distance(p2);
  math::Vector3 p( this->visualMsg->pose().position().x(),
                   this->visualMsg->pose().position().y(),
                   this->visualMsg->pose().position().z() );

  p.z = scale*0.5;

  msgs::Set(this->visualMsg->mutable_pose()->mutable_position(), p);
  msgs::Set(this->visualMsg->mutable_scale(),math::Vector3(scale,scale,scale));
  this->visPub->Publish(*this->visualMsg);
}

void SphereMaker::CreateTheEntity()
{
  msgs::Factory msg;
  msgs::Init(msg, "new_sphere");
  std::ostringstream newModelStr;

  newModelStr << "<gazebo version='1.0'>\
    <model name='" << this->visualMsg->header().str_id() << "_model'>\
    <origin pose='" << this->visualMsg->pose().position().x() << " " 
                    << this->visualMsg->pose().position().y() << " " 
                    << this->visualMsg->scale().x() * 0.5 << " 0 0 0'/>\
    <link name='body'>\
      <inertial mass='1.0'>\
          <inertia ixx='1' ixy='0' ixz='0' iyy='1' iyz='0' izz='1'/>\
      </inertial>\
      <collision name='geom'>\
        <geometry>\
          <sphere radius='" << this->visualMsg->scale().x()*.5 << "'/>\
        </geometry>\
      </collision>\
      <visual name='visual' cast_shadows='true'>\
        <geometry>\
          <sphere radius='" << this->visualMsg->scale().x()*.5 << "'/>\
        </geometry>\
        <material script='Gazebo/Grey'/>\
      </visual>\
    </link>\
  </model>\
  </gazebo>";

  msg.set_sdf( newModelStr.str() );

  msgs::Stamp(this->visualMsg->mutable_header());
  this->visualMsg->set_action( msgs::Visual::DELETE );
  this->visPub->Publish(*this->visualMsg);

  this->makerPub->Publish(msg);
}
