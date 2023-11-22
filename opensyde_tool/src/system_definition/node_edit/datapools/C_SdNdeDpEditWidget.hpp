//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget for system definition update

   \copyright   Copyright 2016 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------

#ifndef C_SDNDEDPEDITWIDGET_HPP
#define C_SDNDEDPEDITWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.hpp"

#include "C_NagUseCaseWidget.hpp"
#include "C_OscNodeDataPool.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */

namespace Ui
{
class C_SdNdeDpEditWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpEditWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpEditWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpEditWidget() override;
   void SetNode(const uint32_t & oru32_NodeIndex);
   void OpenDetail(const int32_t os32_DataPoolIndex, const int32_t os32_ListIndex,
                   const int32_t os32_DataElementIndex) const;
   void OpenOverview(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigDataPoolsChanged(void);
   void SigErrorChange(void) const;
   void SigSwitchToHalc(void) const;
   void SigSwitchToCommMessages(const uint32_t ou32_DataPoolIndex) const;
   void SigSwitchToCanOpen(void) const;
   void SigSave(void);
   void SigSaveAs(void);

private:
   //Avoid call
   C_SdNdeDpEditWidget(const C_SdNdeDpEditWidget &);
   C_SdNdeDpEditWidget & operator =(const C_SdNdeDpEditWidget &) &;

   Ui::C_SdNdeDpEditWidget * mpc_Ui;
   uint32_t mu32_NodeIndex;

   void m_OnListsBack(void) const;
   void m_OnListsNavigate(const bool oq_Forwards) const;
   void m_OpenDataPoolContent(const uint32_t ou32_DataPoolIndex) const;
   void m_OnErrorChange(const bool & orq_Error) const;
   void m_OnSizeChange(void) const;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
