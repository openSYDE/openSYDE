//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system definition update

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_SDNDEDPEDITWIDGET_H
#define C_SDNDEDPEDITWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"

#include "C_NagUseCaseWidget.h"
#include "C_OSCNodeDataPool.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdNdeDpEditWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpEditWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpEditWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDpEditWidget();
   void SetNode(const stw_types::uint32 & oru32_NodeIndex);
   void OpenDetail(const stw_types::sint32 os32_DataPoolIndex, const stw_types::sint32 os32_ListIndex,
                   const stw_types::sint32 os32_DataElementIndex) const;
   void OpenOverview(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigDataPoolsChanged(void);
   void SigErrorChange(void) const;
   void SigSwitchToHalc(void) const;
   void SigSwitchToCommMessages(const stw_types::uint32 ou32_DataPoolIndex) const;
   void SigSwitchToCanOpen(void) const;
   void SigSave(void);
   void SigSaveAs(void);

private:
   //Avoid call
   C_SdNdeDpEditWidget(const C_SdNdeDpEditWidget &);
   C_SdNdeDpEditWidget & operator =(const C_SdNdeDpEditWidget &);

   Ui::C_SdNdeDpEditWidget * mpc_Ui;
   stw_types::uint32 mu32_NodeIndex;

   void m_OnListsBack(void) const;
   void m_OnListsNavigate(const bool oq_Forwards) const;
   void m_OpenDataPoolContent(const stw_types::uint32 ou32_DataPoolIndex) const;
   void m_OnErrorChange(const bool & orq_Error) const;
   void m_OnSizeChange(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
