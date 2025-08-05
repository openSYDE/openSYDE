//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for node properties tab content

   Widget for node properties tab content

   \copyright   Copyright 2022 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include "C_UsHandler.hpp"

#include "C_SdNdeNodePropertiesTabContentWidget.hpp"
#include "ui_C_SdNdeNodePropertiesTabContentWidget.h"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;
using namespace stw::opensyde_gui_logic;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out]  opc_Parent    Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeNodePropertiesTabContentWidget::C_SdNdeNodePropertiesTabContentWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdNdeNodePropertiesTabContentWidget)
{
   this->mpc_Ui->setupUi(this);

   // Splitter stretch: if there is more space stretch left widget (i.e. index 0)
   this->mpc_Ui->pc_Splitter->setStretchFactor(0, 10);
   this->mpc_Ui->pc_Splitter->setStretchFactor(1, 0);

   connect(this->mpc_Ui->pc_NodePropWidget, &C_SdNdeNodePropertiesWidget::SigChanged,
           this, &C_SdNdeNodePropertiesTabContentWidget::SigChanged);
   connect(this->mpc_Ui->pc_NodePropWidget, &C_SdNdeNodePropertiesWidget::SigErrorChange,
           this, &C_SdNdeNodePropertiesTabContentWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_NodePropWidget, &C_SdNdeNodePropertiesWidget::SigNameChanged,
           this, &C_SdNdeNodePropertiesTabContentWidget::SigNameChanged);
   connect(this->mpc_Ui->pc_NodePropWidget, &C_SdNdeNodePropertiesWidget::SigBusBitrateClicked,
           this, &C_SdNdeNodePropertiesTabContentWidget::SigBusBitrateClicked);
   connect(this->mpc_Ui->pc_NodePropWidget, &C_SdNdeNodePropertiesWidget::SigNodePropChanged,
           this, &C_SdNdeNodePropertiesTabContentWidget::SigNodePropChanged);
   connect(this->mpc_Ui->pc_NodePropWidget, &C_SdNdeNodePropertiesWidget::SigNodeXappSupportChanged,
           this, &C_SdNdeNodePropertiesTabContentWidget::SigNodeXappSupportChanged);
   connect(this->mpc_Ui->pc_WidgetApplications, &C_SdNdeDbViewWidget::SigErrorChange, this,
           &C_SdNdeNodePropertiesTabContentWidget::SigErrorChange);
   connect(this->mpc_Ui->pc_WidgetApplications, &C_SdNdeDbViewWidget::SigOwnedDataPoolsChanged, this,
           &C_SdNdeNodePropertiesTabContentWidget::SigOwnedDataPoolsChanged);
   connect(this->mpc_Ui->pc_WidgetApplications, &C_SdNdeDbViewWidget::SigHalcLoadedFromTsp, this,
           &C_SdNdeNodePropertiesTabContentWidget::SigHalcLoadedFromTsp);
   connect(this->mpc_Ui->pc_WidgetApplications, &C_SdNdeDbViewWidget::SigUpdateTrigger, this,
           &C_SdNdeNodePropertiesTabContentWidget::m_UpdateTrigger);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdNdeNodePropertiesTabContentWidget::~C_SdNdeNodePropertiesTabContentWidget()
{
   delete this->mpc_Ui;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Set node index

   \param[in]  ou32_NodeIndex    Node index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesTabContentWidget::SetNodeIndex(const uint32_t ou32_NodeIndex)
{
   this->mpc_Ui->pc_WidgetApplications->SetNodeIndex(ou32_NodeIndex);
   this->mpc_Ui->pc_NodePropWidget->SetNodeId(ou32_NodeIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Update applications
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesTabContentWidget::UpdateApplications() const
{
   this->mpc_Ui->pc_WidgetApplications->UpdateApplications();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Save ui data to node

   Is called from outside
      - on system definition save
      - on page change
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesTabContentWidget::SaveToData() const
{
   this->mpc_Ui->pc_NodePropWidget->SaveToData();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Selects the node name in the text edit for fast editing
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesTabContentWidget::SelectName() const
{
   this->mpc_Ui->pc_NodePropWidget->SelectName();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Scrolls to the application with the index ou32_ApplicationIndex

   \param[in]  ou32_ApplicationIndex   Index of application
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesTabContentWidget::ShowApplication(const uint32_t ou32_ApplicationIndex) const
{
   this->mpc_Ui->pc_WidgetApplications->ShowApplication(ou32_ApplicationIndex);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Wrapper to call C_SdNdeDbViewWidget::AddFromTsp()

   \param[in]  oq_IsNewNode   Is Node new or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesTabContentWidget::AddFromTsp(const bool oq_IsNewNode)
{
   this->mpc_Ui->pc_WidgetApplications->AddFromTsp(oq_IsNewNode);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Save user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesTabContentWidget::SaveUserSettings()
{
   const QList<int32_t> c_Sizes = this->mpc_Ui->pc_Splitter->sizes();

   if (c_Sizes.size() > 0)
   {
      C_UsHandler::h_GetInstance()->SetSdNodeEditSplitterHorizontal(c_Sizes.at(0));
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Load user settings
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesTabContentWidget::LoadUserSettings()
{
   const int32_t s32_FirstSegmentWidth = C_UsHandler::h_GetInstance()->GetSdNodeEditSplitterHorizontal();

   this->mpc_Ui->pc_Splitter->SetFirstSegment(s32_FirstSegmentWidth);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  GUI Update trigger on TSP V3 Import

   \param[in]  ou32_NodeIndex             Node index
   \param[in]  oq_OnlyUpdateProperties    Only update properties
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdNdeNodePropertiesTabContentWidget::m_UpdateTrigger(const uint32_t ou32_NodeIndex,
                                                            const bool oq_OnlyUpdateProperties)
{
   this->SetNodeIndex(ou32_NodeIndex);
   if (oq_OnlyUpdateProperties == false)
   {
      Q_EMIT (this->SigUpdateTrigger());
   }
}
