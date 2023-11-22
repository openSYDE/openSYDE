//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Edit widget for any datapool list
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDPLISTEDITWIDGET_HPP
#define C_SDNDEDPLISTEDITWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include <QKeyEvent>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDpListEditWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDpListEditWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDpListEditWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDpListEditWidget(void) override;

   void Clear(void) const;
   void InitStaticNames(void) const;
   void SetDefaultVisible(void) const;
   void OpenDetail(const int32_t os32_ListIndex, const int32_t os32_DataElementIndex) const;
   void SetDataPool(const uint32_t ou32_NodeIndex, const uint32_t ou32_DataPoolIndex) const;

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
   void SigEdit(const uint32_t ou32_DataPoolIndex, const uint32_t ou32_ListIndex);

protected:
   void keyPressEvent(QKeyEvent * const opc_Event) override;

private:
   Ui::C_SdNdeDpListEditWidget * mpc_Ui;

   void m_OnBack(void);
   void m_OnPrevious(void);
   void m_OnNext(void);
   void m_UpdateButtonTooltips(const uint32_t ou32_NodeIndex, const uint32_t ou32_DatapoolIndex) const;

   //Avoid call
   C_SdNdeDpListEditWidget(const C_SdNdeDpListEditWidget &);
   C_SdNdeDpListEditWidget & operator =(const C_SdNdeDpListEditWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
