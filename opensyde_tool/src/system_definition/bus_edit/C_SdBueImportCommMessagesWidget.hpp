//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget to import COMM messages
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDIMPORTCOMMMESSAGESWIDGET_H
#define C_SDIMPORTCOMMMESSAGESWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include "precomp_headers.hpp"

#include <limits>
#include <fstream>
#include <QFileInfo>
#include <QDir>
#include <QGraphicsView>
#include <QScreen>
#include <QProcess>
#include <QFileDialog>
#include <QDateTime>
#include "TglUtils.hpp"
#include "stwerrors.hpp"
#include "constants.hpp"
#include "C_OscUtils.hpp"
#include "C_OgeWiUtil.hpp"
#include "C_GtGetText.hpp"
#include "C_OscLoggingHandler.hpp"
#include "C_RtfExportWidget.hpp"
#include "TglFile.hpp"
#include "C_OscXmlParser.hpp"
#include "C_OscProject.hpp"
#include "C_PuiProject.hpp"
#include "C_Uti.hpp"
#include "C_PuiUtil.hpp"
#include "C_SdTopologyWidget.hpp"
#include "C_OgeWiCustomMessage.hpp"
#include "ui_C_SdBueImportCommMessagesWidget.h"

namespace Ui
{
class C_SdImportCommMessagesWidget;
}

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdBueImportCommMessagesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueImportCommMessagesWidget(opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                            const stw::scl::C_SclString & orc_Name);
   ~C_SdBueImportCommMessagesWidget(void) override;

   void SetDefaultCommunicationProtocol(const opensyde_core::C_OscCanProtocol::E_Type & ore_ProtocolType);
   opensyde_core::C_OscCanProtocol::E_Type GetSelectedCommunicationProtocol(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigContinue(const stw::opensyde_core::C_OscCanProtocol::E_Type oe_SelectedProtocol);

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   /*************Functions*****************/
   void m_InitStaticNames(const scl::C_SclString & orc_Name) const;
   void m_AddPrototcolsToCombobox();
   void m_OkClicked(void);
   void m_CancelClicked(void);

   /*************Variables*****************/
   Ui::C_SdImportCommMessagesWidget * mpc_Ui;
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;

   stw::opensyde_core::C_OscCanProtocol::E_Type me_SelectedCommProtocol;
   //Avoid call
   C_SdBueImportCommMessagesWidget(const C_SdBueImportCommMessagesWidget &);
   C_SdBueImportCommMessagesWidget & operator =(const C_SdBueImportCommMessagesWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
