//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for showing message layout

   Widget for showing message layout

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

#include <QScrollBar>

#include "stwtypes.h"

#include "C_SdBueMlvWidget.h"
#include "ui_C_SdBueMlvWidget.h"

#include "C_SdBueMlvGraphicsView.h"

/* -- Used Namespaces ------------------------------------------------------ */
using namespace stw_types;
using namespace stw_opensyde_gui;
using namespace stw_opensyde_core;

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
C_SdBueMlvWidget::C_SdBueMlvWidget(QWidget * const opc_Parent) :
   QWidget(opc_Parent),
   mpc_Ui(new Ui::C_SdBueMlvWidget)
{
   mpc_Ui->setupUi(this);

   // create scene for graphics view
   this->mpc_Scene = new C_SdBueMlvGraphicsScene();

   this->mpc_Ui->pc_GraphicsView->setScene(this->mpc_Scene);

   this->mpc_Scene->setSceneRect(0.0, 0.0, static_cast<float64>(this->mpc_Ui->pc_GraphicsView->width()),
                                 static_cast<float64>(this->mpc_Ui->pc_GraphicsView->height()));

   this->mpc_Ui->pc_GraphicsView->horizontalScrollBar()->setValue(0);
   this->mpc_Ui->pc_GraphicsView->verticalScrollBar()->setValue(0);

   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigChangeCursor, this, &C_SdBueMlvWidget::m_ChangeCursor);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigMessageUpdated, this, &C_SdBueMlvWidget::m_OnMessageUpdated);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigSignalActivated, this, &C_SdBueMlvWidget::m_OnSignalActivated);
   // MLV actions
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigAddSignal, this, &C_SdBueMlvWidget::SigAddSignal);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigCopySignal, this, &C_SdBueMlvWidget::SigCopySignal);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigCutSignal, this, &C_SdBueMlvWidget::SigCutSignal);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigPasteSignal, this, &C_SdBueMlvWidget::SigPasteSignal);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigDeleteSignal, this, &C_SdBueMlvWidget::SigDeleteSignal);
   //Connect for MLV tool tip
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigShowToolTip, this->mpc_Ui->pc_GraphicsView,
           &C_SdBueMlvGraphicsView::ShowToolTip);
   connect(this->mpc_Scene, &C_SdBueMlvGraphicsScene::SigHideToolTip, this->mpc_Ui->pc_GraphicsView,
           &C_SdBueMlvGraphicsView::HideToolTip);
   connect(this->mpc_Ui->pc_GraphicsView, &C_SdBueMlvGraphicsView::SigShowToolTip, this->mpc_Scene,
           &C_SdBueMlvGraphicsScene::DisplayToolTip);
}

//-----------------------------------------------------------------------------
/*!
   \brief   default destructor

   Clean up.

   \created     04.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
C_SdBueMlvWidget::~C_SdBueMlvWidget(void)
{
   delete this->mpc_Ui;
   delete this->mpc_Scene;
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set message sync manager

   \param[in,out] opc_Value Message sync manager

   \created     25.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMlvWidget::SetMessageSyncManager(stw_opensyde_gui_logic::C_PuiSdNodeCanMessageSyncManager * const opc_Value)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetMessageSyncManager(opc_Value);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Set new com protocol

   \param[in] ore_Value New value

   \created     26.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMlvWidget::SetComProtocol(const stw_opensyde_core::C_OSCCanProtocol::E_Type & ore_Value)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetComProtocol(ore_Value);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Select message

   \param[in] orc_MessageId Message identification indices

   \created     21.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMlvWidget::SelectMessage(const C_OSCCanMessageIdentificationIndices & orc_MessageId)
{
   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetMessage(orc_MessageId);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Select signal

   \param[in] orc_MessageId     Message identification indices
   \param[in] oru32_SignalIndex Signal index

   \created     17.01.2018  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueMlvWidget::SelectSignal(const C_OSCCanMessageIdentificationIndices & orc_MessageId,
                                    const uint32 ou32_SignalIndex)
{
   this->SelectMessage(orc_MessageId);

   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->SetSignal(ou32_SignalIndex);
   }
}

//-----------------------------------------------------------------------------
/*!
   \brief   Overrided resize event

   Adapting of the scene rectangle

   \param[in,out] opc_Event  Pointer to resize event

   \created     05.04.2017  STW/B.Bayer
*/
//-----------------------------------------------------------------------------
void C_SdBueMlvWidget::resizeEvent(QResizeEvent * const opc_Event)
{
   QWidget::resizeEvent(opc_Event);

   if (this->mpc_Scene != NULL)
   {
      this->mpc_Scene->setSceneRect(0.0, 0.0,
                                    static_cast<float64>(this->mpc_Ui->pc_GraphicsView->width()),
                                    static_cast<float64>(this->mpc_Ui->pc_GraphicsView->height()));
   }
}

//-----------------------------------------------------------------------------
void C_SdBueMlvWidget::m_ChangeCursor(const Qt::CursorShape oe_Cursor)
{
   this->setCursor(oe_Cursor);
}

//-----------------------------------------------------------------------------
/*!
   \brief   On change of signal position in message

   \created     27.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMlvWidget::m_OnMessageUpdated(void)
{
   Q_EMIT this->SigMessageUpdated();
}

//-----------------------------------------------------------------------------
/*!
   \brief   On change of active signal via selector widget

   \param[in] ou32_SignalIndex Active signal index

   \created     27.04.2017  STW/M.Echtler
*/
//-----------------------------------------------------------------------------
void C_SdBueMlvWidget::m_OnSignalActivated(const uint32 ou32_SignalIndex)
{
   Q_EMIT this->SigSignalActivated(ou32_SignalIndex);
}
