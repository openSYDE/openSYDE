//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       HALC overview widget
   \copyright   Copyright 2020 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEHALCOVWIDGET_HPP
#define C_SDNDEHALCOVWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>
#include "stwtypes.hpp"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeHalcOvWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeHalcOvWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeHalcOvWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeHalcOvWidget(void) override;

   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void InitStaticNames(void) const;
   void UpdateData(void);
   void SetNodeIndex(const uint32_t ou32_NodeIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChannelSelected(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                           const bool oq_UseChannelIndex);

private:
   Ui::C_SdNdeHalcOvWidget * mpc_Ui;

   //Avoid call
   C_SdNdeHalcOvWidget(const C_SdNdeHalcOvWidget &);
   C_SdNdeHalcOvWidget & operator =(const C_SdNdeHalcOvWidget &) &;

   void m_UpdateUi(void);
   void m_OnChannelSelected(const uint32_t ou32_DomainIndex, const uint32_t ou32_ChannelIndex,
                            const bool oq_UseChannelIndex);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
