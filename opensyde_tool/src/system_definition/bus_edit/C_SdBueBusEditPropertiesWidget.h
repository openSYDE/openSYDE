//-----------------------------------------------------------------------------
/*!
   \internal
   \file
   \brief       Widget for editing bus properties

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     24.03.2017  STW/B.Bayer
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUEBUSEDITPROPERTIESWIDGET_H
#define C_SDBUEBUSEDITPROPERTIESWIDGET_H

/* -- Includes ------------------------------------------------------------- */

#include <QWidget>

#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */

namespace Ui
{
class C_SdBueBusEditPropertiesWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */

class C_SdBueBusEditPropertiesWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueBusEditPropertiesWidget(QWidget * const opc_Parent = NULL);
   virtual ~C_SdBueBusEditPropertiesWidget(void);

   void InitStaticNames(void) const;
   void SelectName(void) const;
   void SetBusId(const stw_types::uint32 ou32_BusIndex);
   void SaveToData(void) const;

private:
   //Avoid call
   C_SdBueBusEditPropertiesWidget(const C_SdBueBusEditPropertiesWidget &);
   C_SdBueBusEditPropertiesWidget & operator =(const C_SdBueBusEditPropertiesWidget &);

   Ui::C_SdBueBusEditPropertiesWidget * mpc_Ui;
   stw_types::uint32 mu32_BusIndex;

   QString m_GetComboBoxString(const stw_types::uint32 ou32_Bitrate) const;
   stw_types::uint32 m_GetBitrateFromComboBoxString(const QString & orc_Entry) const;

   void m_CheckBusName(void);
   void m_CheckBusId(void) const;
   void m_TrimmBusName(void) const;
   void m_RegisterChange(void);
   void m_RegisterNameChange(void);
   void m_RegisterIdChange(void);
   void m_LoadFromData(void);
   void m_CanBitrateFixed(void) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigChanged(void);
   void SigNameChanged(const QString & orc_Name, const QString & orc_SubItemName = "");
};
}

#endif
