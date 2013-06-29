/*
 * Copyright 2012 Open Source Robotics Foundation
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

//#include "gazebo/rendering/Scene.hh"
//#include "gazebo/rendering/UserCamera.hh"

#include "gazebo/rendering/DynamicLines.hh"
#include "gazebo/rendering/Scene.hh"
#include "gazebo/rendering/Visual.hh"
#include "gazebo/rendering/UserCamera.hh"

#include "gazebo/gui/Gui.hh"
#include "gazebo/gui/MouseEventHandler.hh"
#include "gazebo/gui/GuiEvents.hh"
#include "gazebo/gui/model/JointMaker.hh"
#include "gazebo/gui/model/ModelEditorPalette.hh"

using namespace gazebo;
using namespace gui;

/////////////////////////////////////////////////
ModelEditorPalette::ModelEditorPalette(QWidget *_parent)
    : QWidget(_parent)
{
  QVBoxLayout *mainLayout = new QVBoxLayout;

  this->modelTreeWidget = new QTreeWidget();
  this->modelTreeWidget->setColumnCount(1);
  this->modelTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  this->modelTreeWidget->header()->hide();
  connect(this->modelTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
      this, SLOT(OnModelSelection(QTreeWidgetItem *, int)));

/*  QFrame *frame = new QFrame;
  QVBoxLayout *frameLayout = new QVBoxLayout;
  frameLayout->addWidget(this->modelTreeWidget, 0);
  frameLayout->setContentsMargins(0, 0, 0, 0);
  frame->setLayout(frameLayout);*/
  mainLayout->addWidget(this->modelTreeWidget);

  // Create a top-level tree item for the path
  this->modelSettingsItem =
    new QTreeWidgetItem(static_cast<QTreeWidgetItem*>(0),
        QStringList(QString("Model Settings")));
  this->modelTreeWidget->addTopLevelItem(this->modelSettingsItem);

  this->modelItem =
    new QTreeWidgetItem(static_cast<QTreeWidgetItem*>(0),
        QStringList(QString("Parts and Joints")));
  this->modelTreeWidget->addTopLevelItem(this->modelItem);

  QTreeWidgetItem *modelChildItem =
    new QTreeWidgetItem(static_cast<QTreeWidgetItem*>(0));
  this->modelItem->addChild(modelChildItem);

  // Parts and joints buttons
  QWidget *modelWidget = new QWidget;
  QVBoxLayout *modelLayout = new QVBoxLayout;
  QGridLayout *partsLayout = new QGridLayout;
  QLabel *partsLabel = new QLabel(tr("Parts"));

  // cylinder button
  QPushButton *cylinderButton = new QPushButton(tr("Cylinder"), this);
  cylinderButton->setCheckable(true);
  cylinderButton->setChecked(false);
  connect(cylinderButton, SIGNAL(clicked()), this, SLOT(OnCylinder()));

  // Sphere button
  QPushButton *sphereButton = new QPushButton(tr("Sphere"), this);
  sphereButton->setCheckable(true);
  sphereButton->setChecked(false);
  connect(sphereButton, SIGNAL(clicked()), this, SLOT(OnSphere()));

  // Box button
  QPushButton *boxButton = new QPushButton(tr("Box"), this);
  boxButton->setCheckable(true);
  boxButton->setChecked(false);
  connect(boxButton, SIGNAL(clicked()), this, SLOT(OnBox()));

  QButtonGroup *partsButtonGroup = new QButtonGroup;
  partsButtonGroup->addButton(cylinderButton);
  partsButtonGroup->addButton(sphereButton);
  partsButtonGroup->addButton(boxButton);

  partsLayout->addWidget(partsLabel, 0, 0);
  partsLayout->addWidget(cylinderButton, 1, 0);
  partsLayout->addWidget(sphereButton, 1, 1);
  partsLayout->addWidget(boxButton, 1, 2);

  QGridLayout *jointsLayout = new QGridLayout;
  QLabel *jointsLabel = new QLabel(tr("Joints"));

  // Fixed joint button
  QPushButton *fixedJointButton = new QPushButton(tr("Fixed"), this);
  fixedJointButton->setCheckable(true);
  fixedJointButton->setChecked(false);
  connect(fixedJointButton, SIGNAL(clicked()), this, SLOT(OnFixedJoint()));

  // Hinge joint button
  QPushButton *hingeJointButton = new QPushButton(tr("Hinge"), this);
  hingeJointButton->setCheckable(true);
  hingeJointButton->setChecked(false);
  connect(hingeJointButton, SIGNAL(clicked()), this, SLOT(OnHingeJoint()));

  // Hinge2 joint button
  QPushButton *hinge2JointButton = new QPushButton(tr("Hinge2"), this);
  hinge2JointButton->setCheckable(true);
  hinge2JointButton->setChecked(false);
  connect(hinge2JointButton, SIGNAL(clicked()), this, SLOT(OnHinge2Joint()));

  // slider joint button
  QPushButton *sliderJointButton = new QPushButton(tr("Slider"), this);
  sliderJointButton->setCheckable(true);
  sliderJointButton->setChecked(false);
  connect(sliderJointButton, SIGNAL(clicked()), this, SLOT(OnSliderJoint()));

  // Screw joint button
  QPushButton *screwJointButton = new QPushButton(tr("Screw"), this);
  screwJointButton->setCheckable(true);
  screwJointButton->setChecked(false);
  connect(screwJointButton, SIGNAL(clicked()), this, SLOT(OnScrewJoint()));

  // Universal joint button
  QPushButton *universalJointButton = new QPushButton(tr("Universal"), this);
  universalJointButton->setCheckable(true);
  universalJointButton->setChecked(false);
  connect(universalJointButton, SIGNAL(clicked()), this,
      SLOT(OnUniversalJoint()));

  // Ball joint button
  QPushButton *ballJointButton = new QPushButton(tr("Ball"), this);
  ballJointButton->setCheckable(true);
  ballJointButton->setChecked(false);
  connect(ballJointButton, SIGNAL(clicked()), this, SLOT(OnBallJoint()));


  QButtonGroup *jointsButtonGroup = new QButtonGroup;
  jointsButtonGroup->addButton(fixedJointButton);
  jointsButtonGroup->addButton(sliderJointButton);
  jointsButtonGroup->addButton(hingeJointButton);
  jointsButtonGroup->addButton(hinge2JointButton);
  jointsButtonGroup->addButton(screwJointButton);
  jointsButtonGroup->addButton(universalJointButton);
  jointsButtonGroup->addButton(ballJointButton);

  jointsLayout->addWidget(jointsLabel, 0, 0);
  jointsLayout->addWidget(fixedJointButton, 1, 0);
  jointsLayout->addWidget(sliderJointButton, 1, 1);
  jointsLayout->addWidget(hingeJointButton, 1, 2);
  jointsLayout->addWidget(hinge2JointButton, 2, 0);
  jointsLayout->addWidget(screwJointButton, 2, 1);
  jointsLayout->addWidget(universalJointButton, 2, 2);
  jointsLayout->addWidget(ballJointButton, 3, 0);

  modelLayout->addLayout(partsLayout);
  modelLayout->addLayout(jointsLayout);
  modelWidget->setLayout(modelLayout);
  this->modelTreeWidget->setItemWidget(modelChildItem, 0, modelWidget);
  this->modelItem->setExpanded(true);
  modelChildItem->setExpanded(true);

  // plugin
  this->pluginItem =
    new QTreeWidgetItem(static_cast<QTreeWidgetItem*>(0),
        QStringList(QString("Plugin")));
    this->modelTreeWidget->addTopLevelItem(this->pluginItem);

  this->modelCreator = new ModelCreator();
  this->jointMaker = new JointMaker();

  mainLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

  this->setObjectName("modelEditorPalette");
  this->setLayout(mainLayout);
  this->layout()->setContentsMargins(0, 0, 0, 0);
}

/////////////////////////////////////////////////
ModelEditorPalette::~ModelEditorPalette()
{
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnModelSelection(QTreeWidgetItem *_item,
                                         int /*_column*/)
{
  if (_item)
  {
   /* std::string path, filename;

    if (_item->parent())
      path = _item->parent()->text(0).toStdString() + "/";

    path = _item->data(0, Qt::UserRole).toString().toStdString();

    if (!path.empty())
    {
      //QApplication::setOverrideCursor(Qt::BusyCursor);
      //filename = common::ModelDatabase::Instance()->GetModelFile(path);
      //gui::Events::createEntity("model", filename);

      //this->fileTreeWidget->clearSelection();
      //QApplication::setOverrideCursor(Qt::ArrowCursor);
    }*/
  }
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnCylinder()
{
  this->modelCreator->CreatePart(ModelCreator::PART_CYLINDER);
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnSphere()
{
  this->modelCreator->CreatePart(ModelCreator::PART_SPHERE);
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnBox()
{
  this->modelCreator->CreatePart(ModelCreator::PART_BOX);
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnFixedJoint()
{
  this->jointMaker->CreateJoint(JointMaker::JOINT_FIXED);
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnHingeJoint()
{
  this->jointMaker->CreateJoint(JointMaker::JOINT_HINGE);
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnHinge2Joint()
{
  this->jointMaker->CreateJoint(JointMaker::JOINT_HINGE2);
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnSliderJoint()
{
  this->jointMaker->CreateJoint(JointMaker::JOINT_SLIDER);
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnScrewJoint()
{
  this->jointMaker->CreateJoint(JointMaker::JOINT_SCREW);
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnUniversalJoint()
{
  this->jointMaker->CreateJoint(JointMaker::JOINT_UNIVERSAL);
}

/////////////////////////////////////////////////
void ModelEditorPalette::OnBallJoint()
{
  this->jointMaker->CreateJoint(JointMaker::JOINT_BALL);
}
