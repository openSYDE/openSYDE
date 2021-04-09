//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for selecting a system definition node for DCF/EDS file import (header)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEDCFEDSIMPORTNODESELECTWIDGET_H
#define C_CIEDCFEDSIMPORTNODESELECTWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"
#include "C_CieDcfEdsImportNodeSelectWidget.h"
#include "C_OgePopUpDialog.h"
#include "C_CieConverter.h"

namespace Ui
{
class C_CieDcfEdsImportNodeSelectWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieDcfEdsImportNodeSelectWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CieDcfEdsImportNodeSelectWidget(stw_opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                              const QString & orc_FilePath, const stw_types::uint32 ou32_BusIndex);
   virtual ~C_CieDcfEdsImportNodeSelectWidget();

   void InitStaticNames(void);

   stw_types::sint32 GetNodeSelection(stw_types::uint32 & oru32_NodeIndex,
                                      stw_types::uint32 & oru32_InterfaceIndex) const;

protected:
   virtual void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_CieDcfEdsImportNodeSelectWidget & operator =(const C_CieDcfEdsImportNodeSelectWidget &);
   C_CieDcfEdsImportNodeSelectWidget(const C_CieDcfEdsImportNodeSelectWidget &);

   std::vector<stw_types::uint32> mc_NodeIndexes;
   std::vector<stw_types::uint32> mc_InterfaceIndexes;

   void m_OkClicked(void) const;
   void m_CancelClicked(void) const;

   void m_FillUpComboBox(const stw_types::uint32 ou32_BusIndex);

   Ui::C_CieDcfEdsImportNodeSelectWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw_opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
};
}

#endif
