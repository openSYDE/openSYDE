//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Edit widget for any datapool list
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTEDITWIDGET_H
#define C_SDNDEDPLISTEDITWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QKeyEvent>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDpListEditWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListEditWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpListEditWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpListEditWidget(void);

   void Clear(void) const;
   void InitStaticNames(void) const;
   void SetDefaultVisible(void) const;
   void OpenDetail(const stw_types::sint32 os32_ListIndex, const stw_types::sint32 os32_DataElementIndex) const;
   void SetDataPool(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_DataPoolIndex) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigBack(void);
   void SigNavigate(const bool oq_Forwards); // true/false = forwards/backwards = navigate to next/previous Datapool
   void SigChanged(void);
   void SigErrorChange(const bool & orq_Error) const;
   void SigSizeChange(void);
   void SigSave(void);
   void SigSaveAs(void);
   void SigEdit(const stw_types::uint32 ou32_DataPoolIndex, const stw_types::uint32 ou32_ListIndex);

protected:
   // The naming of the Qt parameters can't be changed and are not compliant with the naming conventions
   //lint -save -e1960
   virtual void keyPressEvent(QKeyEvent * const opc_Event) override;
   //lint -restore

private:
   Ui::C_SdNdeDpListEditWidget * mpc_Ui;

   void m_OnBack(void);
   void m_OnPrevious(void);
   void m_OnNext(void);
   void m_UpdateButtonTooltips(const stw_types::uint32 ou32_NodeIndex,
                               const stw_types::uint32 ou32_DatapoolIndex) const;

   //Avoid call
   C_SdNdeDpListEditWidget(const C_SdNdeDpListEditWidget &);
   C_SdNdeDpListEditWidget & operator =(const C_SdNdeDpListEditWidget &);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
