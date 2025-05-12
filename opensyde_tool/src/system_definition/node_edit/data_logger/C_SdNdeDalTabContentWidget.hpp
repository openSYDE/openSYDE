//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Main widget for data logger tab content
   \copyright   Copyright 2024 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_SDNDEDALTABCONTENTWIDGET_HPP
#define C_SDNDEDALTABCONTENTWIDGET_HPP

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_SdNdeDalTabContentWidget;
}

namespace stw
{
namespace opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_SdNdeDalTabContentWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdNdeDalTabContentWidget(QWidget * const opc_Parent = NULL);
   ~C_SdNdeDalTabContentWidget(void) override;

   void LoadUserSettings(void) const;
   void SaveUserSettings(void) const;
   void InitStaticNames(void) const;
   void ReloadDataLogger(void);
   void SetNode(const uint32_t ou32_NodeIndex);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736
Q_SIGNALS:
   //lint -restore
   void SigErrorChange(void) const;

private:
   Ui::C_SdNdeDalTabContentWidget * mpc_Ui;
   uint32_t mu32_NodeIndex;

   static const uint32_t mhu32_STATIC_LOG_JOB_INDEX;

   void m_UpdateUi(void);

   //Avoid call
   C_SdNdeDalTabContentWidget(const C_SdNdeDalTabContentWidget &);
   C_SdNdeDalTabContentWidget & operator =(const C_SdNdeDalTabContentWidget &) &;
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
}
} //end of namespace

#endif
