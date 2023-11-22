//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for selecting a system definition node for DCF/EDS file import (header)

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CIEDCFEDSIMPORTNODESELECTWIDGET_HPP
#define C_CIEDCFEDSIMPORTNODESELECTWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"
#include "C_CieDcfEdsImportNodeSelectWidget.hpp"
#include "C_OgePopUpDialog.hpp"
#include "C_CieConverter.hpp"

namespace Ui
{
class C_CieDcfEdsImportNodeSelectWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CieDcfEdsImportNodeSelectWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CieDcfEdsImportNodeSelectWidget(stw::opensyde_gui_elements::C_OgePopUpDialog & orc_Parent,
                                              const QString & orc_FilePath, const uint32_t ou32_BusIndex);
   ~C_CieDcfEdsImportNodeSelectWidget() override;

   void InitStaticNames(void);

   int32_t GetNodeSelection(uint32_t & oru32_NodeIndex, uint32_t & oru32_InterfaceIndex) const;

protected:
   void keyPressEvent(QKeyEvent * const opc_KeyEvent) override;

private:
   //Avoid call
   C_CieDcfEdsImportNodeSelectWidget & operator =(const C_CieDcfEdsImportNodeSelectWidget &) &;
   C_CieDcfEdsImportNodeSelectWidget(const C_CieDcfEdsImportNodeSelectWidget &);

   std::vector<uint32_t> mc_NodeIndexes;
   std::vector<uint32_t> mc_InterfaceIndexes;

   void m_OkClicked(void) const;
   void m_CancelClicked(void) const;

   void m_FillUpComboBox(const uint32_t ou32_BusIndex);

   Ui::C_CieDcfEdsImportNodeSelectWidget * mpc_Ui;
   //lint -e{1725} Only problematic if copy or assignment is allowed
   stw::opensyde_gui_elements::C_OgePopUpDialog & mrc_ParentDialog;
};
}
}

#endif
