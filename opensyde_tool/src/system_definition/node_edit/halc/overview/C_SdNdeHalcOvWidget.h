//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC overview widget
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCOVWIDGET_H
#define C_SDNDEHALCOVWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeHalcOvWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcOvWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeHalcOvWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeHalcOvWidget(void);

   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void InitStaticNames(void) const;
   void UpdateData(void);
   void SetNodeIndex(const stw_types::uint32 ou32_NodeIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChannelSelected(const stw_types::uint32 ou32_DomainIndex, const stw_types::uint32 ou32_ChannelIndex,
                           const bool oq_UseChannelIndex);

private:
   Ui::C_SdNdeHalcOvWidget * mpc_Ui;

   //Avoid call
   C_SdNdeHalcOvWidget(const C_SdNdeHalcOvWidget &);
   C_SdNdeHalcOvWidget & operator =(const C_SdNdeHalcOvWidget &);

   void m_UpdateUi(void);
   void m_OnChannelSelected(const stw_types::uint32 ou32_DomainIndex, const stw_types::uint32 ou32_ChannelIndex,
                            const bool oq_UseChannelIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
