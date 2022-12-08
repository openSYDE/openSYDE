//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Offers system view setup specific visualization and functionality of a node. (implementation)

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsView>

#include "gitypes.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"

#include "C_GtGetText.hpp"
#include "C_ImpUtil.hpp"
#include "C_PuiSdHandler.hpp"
#include "C_PuiSvHandler.hpp"
#include "C_GiSvNodeSyvSetup.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::errors;
using namespace stw::opensyde_gui;
using namespace stw::opensyde_core;
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

   \param[in]       ou32_ViewIndex       Index of system view
   \param[in]       ors32_NodeIndex      Index of data element in system view
   \param[in]       oru64_Id             Unique ID
   \param[in]       orf64_Width          Width of node
   \param[in]       orf64_Height         Height of node
   \param[in,out]   opc_Parent           Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_GiSvNodeSyvSetup::C_GiSvNodeSyvSetup(const uint32_t ou32_ViewIndex, const int32_t & ors32_NodeIndex,
                                       const uint64_t & oru64_Id, const float64_t & orf64_Width,
                                       const float64_t & orf64_Height, QGraphicsItem * const opc_Parent) :
   //lint -e{1938}  static const is guaranteed preinitialized before main
   C_GiSvNodeSyvBase(ou32_ViewIndex, ors32_NodeIndex, oru64_Id, orf64_Width, orf64_Height, opc_Parent),
   mq_EditMode(false)
{
   this->m_InitCheckBox();

   this->mc_DefaultCursor = static_cast<QCursor>(Qt::ArrowCursor);
   this->C_GiSvNodeSyvSetup::RestoreDefaultCursor();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   default destructor

   Clean up.
*/
//----------------------------------------------------------------------------------------------------------------------
//lint -e{1540}  no memory leak because of the parent of mpc_CheckBox and the Qt memory management
C_GiSvNodeSyvSetup::~C_GiSvNodeSyvSetup()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Sets the node connected state

   \param[in]     oq_Connected   Flag if connected or not
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::SetViewConnected(const bool oq_Connected)
{
   C_GiSvNodeSyvBase::SetViewConnected(oq_Connected);

   if (this->mpc_CheckBox != NULL)
   {
      this->mpc_CheckBox->SetChecked(oq_Connected);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Returns the state of the connection to the view

   \return
   true     is connected
   false    is not connected
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_GiSvNodeSyvSetup::IsViewConnected(void) const
{
   return this->mq_ViewConnected;
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Set the mode

   \param[in]     oq_Active      Flag for the edit mode
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::SetEditMode(const bool oq_Active)
{
   this->mq_EditMode = oq_Active;

   if (this->mpc_CheckBox != NULL)
   {
      this->mpc_CheckBox->setVisible(oq_Active);
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Slot function for updating internal data
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::UpdateData(void)
{
   C_PuiSvHandler::h_GetInstance()->SetViewNodeCheckedState(this->mu32_ViewIndex,
                                                            static_cast<uint32_t>(this->ms32_Index),
                                                            this->IsViewConnected());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten mouse press event slot

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::mousePressEvent(QGraphicsSceneMouseEvent * const opc_Event)
{
   if (opc_Event->button() == Qt::LeftButton)
   {
      const QPointF c_Pos = opc_Event->scenePos();
      bool q_ClickRecognized = false;

      if ((this->mpc_CheckBox != NULL) &&
          (this->mq_EditMode == true))
      {
         const QRectF c_RectCheckBox = this->mpc_CheckBox->sceneBoundingRect();

         if (c_RectCheckBox.contains(c_Pos) == true)
         {
            this->SetViewConnected(!this->mpc_CheckBox->IsChecked());
            Q_EMIT this->SigViewConnectionChanged();
            q_ClickRecognized = true;
         }
      }

      if (q_ClickRecognized == true)
      {
         this->m_BlockMoveAndResize();
      }
      else
      {
         if ((this->mpc_CheckBox != NULL) && (this->mq_EditMode == true))
         {
            //New default
            this->SetViewConnected(!this->mpc_CheckBox->IsChecked());
            Q_EMIT this->SigViewConnectionChanged();
            //Block of move necessary (emitted move signals might screw with other items)
            this->m_BlockMoveAndResize();
         }
         else
         {
            C_GiNode::mousePressEvent(opc_Event);
         }
      }
   }
   else
   {
      C_GiNode::mousePressEvent(opc_Event);
   }
}

//----------------------------------------------------------------------------------------------------------------------
void C_GiSvNodeSyvSetup::m_InitCheckBox(void)
{
   const int32_t s32_IconSize = this->m_GetIconSize();

   this->mpc_CheckBox = new C_GiCheckBox(QRect(9, 9, s32_IconSize, s32_IconSize));

   this->mpc_CheckBox->setZValue(mf64_ZORDER_INIT_NODE + 1.0);
   this->mpc_CheckBox->SetChecked(false);
   this->SetDrawWhiteFilter(false);
   this->addToGroup(this->mpc_CheckBox);
   this->mpc_CheckBox->setVisible(false);
}
