//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Table view base class for link interaction wrapping (implementation)

   Table view base class for link interaction wrapping

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QMouseEvent>
#include <QHeaderView>
#include <QApplication>

#include "constants.hpp"
#include "C_TblViewInteraction.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Default constructor

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_TblViewInteraction::C_TblViewInteraction(QWidget * const opc_Parent) :
   C_TblViewToolTipBase(opc_Parent)
{
   this->setMouseTracking(true);

   this->verticalHeader()->installEventFilter(this);
   this->horizontalHeader()->installEventFilter(this);

   connect(this, &C_TblViewInteraction::clicked, this, &C_TblViewInteraction::m_CheckItemClicked);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten mouse move event slot

   Here: handle mouse cursor changes

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblViewInteraction::mouseMoveEvent(QMouseEvent * const opc_Event)
{
   C_TblViewToolTipBase::mouseMoveEvent(opc_Event);
   m_HandleGlobalMousePos(opc_Event->globalPos());
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten event slot

   Here: handle mouse cursor changes

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblViewInteraction::event(QEvent * const opc_Event)
{
   if (opc_Event->type() == QEvent::Leave)
   {
      this->setCursor(Qt::ArrowCursor);
   }
   else if (opc_Event->type() == QEvent::HoverLeave)
   {
      this->setCursor(Qt::ArrowCursor);
   }
   else if (opc_Event->type() == QEvent::HoverMove)
   {
      QHoverEvent * const pc_HoverEvent = dynamic_cast<QHoverEvent * const>(opc_Event);
      if (pc_HoverEvent != NULL)
      {
         m_HandleGlobalMousePos(this->mapToGlobal(pc_HoverEvent->pos()));
      }
   }
   else
   {
      //No handling necessary
   }
   return C_TblViewToolTipBase::event(opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Overwritten event filter slot

   Here: handle mouse cursor changes

   \param[in,out] opc_Object Source sender object
   \param[in,out] opc_Event  Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_TblViewInteraction::eventFilter(QObject * const opc_Object, QEvent * const opc_Event)
{
   if ((opc_Object != this) && (opc_Object != NULL))
   {
      if (opc_Event->type() == QEvent::HoverMove)
      {
         this->setCursor(Qt::ArrowCursor);
      }
   }
   return C_TblViewToolTipBase::eventFilter(opc_Object, opc_Event);
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Handle global position change

   \param[in] orc_GlobalPos Global position
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblViewInteraction::m_HandleGlobalMousePos(const QPoint & orc_GlobalPos)
{
   if ((this->horizontalHeader()->isVisible() == true) &&
       (this->horizontalHeader()->rect().contains(this->horizontalHeader()->mapFromGlobal(orc_GlobalPos)) == true))
   {
      this->setCursor(Qt::ArrowCursor);
   }
   else
   {
      if ((this->verticalHeader()->isVisible() == true) &&
          (this->verticalHeader()->rect().contains(this->verticalHeader()->mapFromGlobal(orc_GlobalPos)) == true))
      {
         this->setCursor(Qt::ArrowCursor);
      }
      else
      {
         QModelIndex c_Index;

         c_Index = this->indexAt(this->viewport()->mapFromGlobal(orc_GlobalPos));
         if (c_Index.isValid())
         {
            if ((this->model()->data(c_Index, ms32_USER_ROLE_INTERACTION_IS_LINK).toBool() == true) &&
                this->model()->flags(c_Index).testFlag(Qt::ItemIsEnabled))
            {
               this->setCursor(Qt::PointingHandCursor);
            }
            else
            {
               this->setCursor(Qt::ArrowCursor);
            }
         }
         else
         {
            this->setCursor(Qt::ArrowCursor);
         }
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief  Check clicked item

   \param[in] orc_Index Clicked item index
*/
//----------------------------------------------------------------------------------------------------------------------
void C_TblViewInteraction::m_CheckItemClicked(const QModelIndex & orc_Index) const
{
   if (orc_Index.data(ms32_USER_ROLE_INTERACTION_IS_LINK).toBool() == true)
   {
      Q_EMIT this->SigLinkClicked(orc_Index);
   }
}
