//-----------------------------------------------------------------------------
/*!
   \file
   \brief       A datapool item widget for the list widget

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.02.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDNDEDATAPOOLSELECTORITEMWIDGET_H
#define C_SDNDEDATAPOOLSELECTORITEMWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include "stwtypes.h"

#include "C_OgeWiWithToolTip.h"
#include "C_OSCNodeDataPool.h"
#include "C_OgeLabToolTipBase.h"
#include "C_SdNdeDataPoolSelectorItemUsageWidget.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SdNdeDataPoolSelectorItemWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_SdNdeDataPoolSelectorItemWidget :
   public stw_opensyde_gui_elements::C_OgeWiWithToolTip
{
   Q_OBJECT

public:
   explicit C_SdNdeDataPoolSelectorItemWidget(const bool oq_UsageViewActive, QWidget * const opc_Parent = NULL);
   virtual ~C_SdNdeDataPoolSelectorItemWidget();

   void InitWidget(void);

   void SetErrorToolTip(const QString & orc_Heading, const QString & orc_Content);
   void SetNumber(const stw_types::uint32 ou32_Number);
   void SetData(const stw_opensyde_core::C_OSCNodeDataPool & orc_OSCDataPool);
   void SetData(const QString & orc_DatapoolName, const QString & orc_DatapoolComment, const bool oq_Safety,
                const stw_types::uint32 ou32_Size, const stw_types::uint32 ou32_Used,
                const stw_types::uint32 ou32_Reserved);
   void SetActive(const bool oq_Active);
   void SetStateConflict(const bool oq_Active);
   QString GetDatapoolName(void) const;
   QString GetDatapoolComment(void) const;
   bool GetStateSafety(void) const;
   bool GetStateConflict(void) const;
   void SetMaximized(const bool oq_Maximized);
   bool GetMaximized(void) const;
   bool GetActive(void) const;
   bool GetUsageViewActive(void) const;
   stw_types::uint32 GetDataPoolSize(void) const;
   stw_types::uint32 GetDataPoolUsedSize(void) const;
   stw_types::uint32 GetDataPoolReservedSize(void) const;

   static const QSize hc_MaximumSize;
   static const QSize hc_MinimumSize;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigUpdateErrorToolTip(void);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void showEvent(QShowEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SdNdeDataPoolSelectorItemWidget(const C_SdNdeDataPoolSelectorItemWidget &);
   C_SdNdeDataPoolSelectorItemWidget & operator =(const C_SdNdeDataPoolSelectorItemWidget &);

   void m_UpdateLabel(void);
   void m_UpdateName(void) const;

   Ui::C_SdNdeDataPoolSelectorItemWidget * mpc_Ui;
   stw_opensyde_gui_elements::C_OgeLabToolTipBase * mpc_LabelStateImg;
   C_SdNdeDataPoolSelectorItemUsageWidget * mpc_UsageBar;
   QPixmap mc_SafetyImg;
   QPixmap mc_ConflictImg;

   bool mq_StateSafety;
   bool mq_StateConflict;
   bool mq_Maximized;
   bool mq_Active;
   const bool mq_UsageViewActive;
   stw_types::uint32 mu32_Size;
   stw_types::uint32 mu32_Used;
   stw_types::uint32 mu32_Reserved;
   stw_types::uint32 mu32_Number;
   QString mc_Name;
};
}

/* -- Extern Global Variables ---------------------------------------------- */
#endif // C_SDNDEDATAPOOLSELECTORITEMWIDGET_H
