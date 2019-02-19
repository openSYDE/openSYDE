//-----------------------------------------------------------------------------
/*!
   \file
   \brief       Bus edit widget (note: main module description should be in .cpp file)

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     01.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUEBUSEDITWIDGET_H
#define C_SDBUEBUSEDITWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>

#include "stwtypes.h"

#include "C_OSCSystemBus.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SdBueBusEditWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdBueBusEditWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueBusEditWidget(const stw_types::uint32 ou32_BusIndex, const stw_types::sintn osn_TabIndex,
                                 QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueBusEditWidget();

   void InitStaticNames(void) const;
   bool WasChanged(void) const;
   void Save(void) const;
   void SetFlag(const stw_types::uint32 ou32_Flag) const;
   void OpenDetail(const stw_types::sint32 os32_NodeIndex, const stw_types::sint32 os32_DataPoolIndex,
                   const stw_types::sint32 os32_ListIndex, const stw_types::sint32 os32_ElementIndex,
                   const stw_types::sint32 os32_Flag) const;
   stw_types::sintn GetTabIndex(void) const;

   const Ui::C_SdBueBusEditWidget * GetUiWidget() const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigErrorChange(void);
   void SigNameChanged(const QString & orc_Name, const QString & orc_SubItemName = "");

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void showEvent(QShowEvent * const opc_Event) override;
   //lint -restore

private:
   //Avoid call
   C_SdBueBusEditWidget(const C_SdBueBusEditWidget &);
   C_SdBueBusEditWidget & operator =(const C_SdBueBusEditWidget &);

   void m_DataChanged(void);
   void m_TabChanged(const stw_types::sintn osn_Index) const;

   Ui::C_SdBueBusEditWidget * mpc_Ui;
   const stw_types::uint32 mu32_BusIndex;
   bool mq_DataChanged;
   stw_opensyde_core::C_OSCSystemBus::E_Type me_BusType;
};
}

#endif
