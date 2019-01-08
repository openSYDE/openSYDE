//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Graphics view for the message layout viewer (implementation)

   detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     04.04.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------

/* -- Includes ------------------------------------------------------------- */
#include "precomp_headers.h"

#include <QHelpEvent>
#include <QMimeData>
#include <QFileInfo>

#include "C_SdBueMlvGraphicsView.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_opensyde_gui;

/* -- Module Global Constants ---------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

/* -- Global Variables ----------------------------------------------------- */

/* -- Module Global Variables ---------------------------------------------- */

/* -- Module Global Function Prototypes ------------------------------------ */

/* -- Implementation ------------------------------------------------------- */

//-----------------------------------------------------------------------------
/*!
   \brief   Default constructor

   Set up GUI with all elements.

   \param[in,out] opc_parent Optional pointer to parent

   \created     04.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdBueMlvGraphicsView::C_SdBueMlvGraphicsView(QWidget * const opc_Parent) :
   QGraphicsView(opc_Parent)
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Default destructor

   Set up GUI with all elements.

   \param[in,out] opc_parent Optional pointer to parent

   \created     04.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdBueMlvGraphicsView::~C_SdBueMlvGraphicsView()
{
}

//-----------------------------------------------------------------------------
/*!
   \brief   Show tool tip

   \param[in] orc_ScenePos Scne position to show tool tip at
   \param[in] orc_Heading  Heading of tool tip
   \param[in] orc_Content  Content of tool tip
   \param[in] oq_Error     Flag if error tool tip

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMlvGraphicsView::ShowToolTip(const QPointF & orc_ScenePos, const QString & orc_Heading,
                                         const QString & orc_Content, const bool oq_Error)
{
   QPointF c_AdaptedScenePos(orc_ScenePos.x(), orc_ScenePos.y());

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

//-----------------------------------------------------------------------------
/*!
   \brief   Hide tool tip

   \created     26.10.2016  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMlvGraphicsView::HideToolTip(void)
{
   this->mc_ToolTip.hide();
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten drag enter event slot

   Here: Do not accept external *.syde file here. It will be handled in NagMainWindow

   \param[in,out] opc_Event Event identification and information

   \created     17.05.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/*!
   \brief   Overwritten default event slot

   Here: Handle tool tip

   \param[in,out] opc_Event Event identification and information

   \return
   True  Event was recognized and processed
   False Event ignored

   \created     16.01.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
bool C_SdBueMlvGraphicsView::event(QEvent * const opc_Event)
{
   if (opc_Event->type() == QEvent::ToolTip)
   {
      //lint -e{929}  false positive in PC-Lint: allowed by MISRA 5-2-2
      QHelpEvent * pc_Help = dynamic_cast<QHelpEvent *>(opc_Event);
      if (pc_Help != NULL)
      {
         Q_EMIT this->SigShowToolTip(this->mapToScene(pc_Help->pos()));
         opc_Event->ignore();
      }
   }
   return QGraphicsView::event(opc_Event);
}
