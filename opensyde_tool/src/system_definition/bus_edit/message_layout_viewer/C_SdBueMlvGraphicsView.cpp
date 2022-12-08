//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Graphics view for the message layout viewer (implementation)

   detailed description

   \copyright   Copyright 2017 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <QHelpEvent>
#include <QMimeData>
#include <QFileInfo>

#include "C_SdBueMlvGraphicsView.hpp"

/* -- Used Namespaces ----------------------------------------------------------------------------------------------- */
using namespace stw::opensyde_gui;

/* -- Module Global Constants --------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

/* -- Global Variables ---------------------------------------------------------------------------------------------- */

/* -- Module Global Variables --------------------------------------------------------------------------------------- */

/* -- Module Global Function Prototypes ----------------------------------------------------------------------------- */

/* -- Implementation ------------------------------------------------------------------------------------------------ */

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_Parent Optional pointer to parent
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvGraphicsView::C_SdBueMlvGraphicsView(QWidget * const opc_Parent) :
   QGraphicsView(opc_Parent)
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Default destructor

   Set up GUI with all elements.
*/
//----------------------------------------------------------------------------------------------------------------------
C_SdBueMlvGraphicsView::~C_SdBueMlvGraphicsView()
{
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Show tool tip

   \param[in] orc_ScenePos Scne position to show tool tip at
   \param[in] orc_Heading  Heading of tool tip
   \param[in] orc_Content  Content of tool tip
   \param[in] oq_Error     Flag if error tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsView::ShowToolTip(const QPointF & orc_ScenePos, const QString & orc_Heading,
                                         const QString & orc_Content, const bool oq_Error)
{
   const QPointF c_AdaptedScenePos(orc_ScenePos.x(), orc_ScenePos.y());

   this->mc_ToolTip.SetHeading(orc_Heading);
   this->mc_ToolTip.SetContent(orc_Content);
   if (oq_Error == true)
   {
      this->mc_ToolTip.SetType(C_NagToolTip::eERROR);
   }
   else
   {
      this->mc_ToolTip.SetType(C_NagToolTip::eDEFAULT);
   }
   this->mc_ToolTip.show();
   this->mc_ToolTip.DoMove(this->mapToGlobal(this->mapFromScene(c_AdaptedScenePos)));
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Hide tool tip
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsView::HideToolTip(void)
{
   this->mc_ToolTip.hide();
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten drag enter event slot

   Here: Do not accept external *.syde file here. It will be handled in NagMainWindow

   \param[in,out] opc_Event Event identification and information
*/
//----------------------------------------------------------------------------------------------------------------------
void C_SdBueMlvGraphicsView::dragEnterEvent(QDragEnterEvent * const opc_Event)
{
   QGraphicsView::dragEnterEvent(opc_Event);

   if (opc_Event->mimeData()->hasUrls() == true)
   {
      const QFileInfo c_File(opc_Event->mimeData()->text());

      //Manually prevent accepting openSYDE files as image
      if (c_File.completeSuffix().compare("syde") == 0)
      {
         opc_Event->ignore();
      }
   }
}

//----------------------------------------------------------------------------------------------------------------------
/*! \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored
*/
//----------------------------------------------------------------------------------------------------------------------
bool C_SdBueMlvGraphicsView::event(QEvent * const opc_Event)
{
   if (opc_Event->type() == QEvent::ToolTip)
   {
      QHelpEvent * const pc_Help = dynamic_cast<QHelpEvent *>(opc_Event);
      if (pc_Help != NULL)
      {
         Q_EMIT this->SigShowToolTip(this->mapToScene(pc_Help->pos()));
         opc_Event->ignore();
      }
   }
   return QGraphicsView::event(opc_Event);
}
