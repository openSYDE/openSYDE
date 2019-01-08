//-----------------------------------------------------------------------------
/*!
   \file
   \brief       One message receiver edit entry (header)

   See cpp file for detailed description

   \implementation
   project     openSYDE
   copyright   STW (c) 1999-20xx
   license     use only under terms of contract / confidential

   created     22.03.2018  STW/M.Echtler
   \endimplementation
*/
//-----------------------------------------------------------------------------
#ifndef C_SDBUEMESSAGERXENTRY_H
#define C_SDBUEMESSAGERXENTRY_H

/* -- Includes ------------------------------------------------------------- */
#include <QIcon>
#include <QWidget>
#include "stwtypes.h"

/* -- Namespace ------------------------------------------------------------ */
namespace Ui
{
class C_SdBueMessageRxEntry;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ----------------------------------------------------- */

/* -- Types ---------------------------------------------------------------- */
class C_SdBueMessageRxEntry :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_SdBueMessageRxEntry(QWidget * const opc_Parent = NULL);
   ~C_SdBueMessageRxEntry(void);

   void InitStaticNames(void) const;
   void Init(const QString & orc_EntryName, const stw_types::uint32 ou32_NodeIndex,
             const stw_types::uint32 ou32_InterfaceIndex, const bool oq_UseAutoReceiveTimeoutFlag,
             const stw_types::uint32 ou32_ReceiveTimeoutValue);
   void SetLastKnownCycleTimeValue(const stw_types::uint32 ou32_Value);
   void SetAlwaysHideTimeout(const bool oq_Hide);
   bool DoesMatch(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex) const;
   void SetChecked(const bool oq_Checked) const;

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   //lint -restore
   void SigNodeToggled(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                       const bool oq_Checked);
   void SigNodeUseAutoReceiveTimeout(const stw_types::uint32 ou32_NodeIndex,
                                     const stw_types::uint32 ou32_InterfaceIndex, const bool oq_UseAutoReceiveTimeout);
   void SigNodeReceiveTimeout(const stw_types::uint32 ou32_NodeIndex, const stw_types::uint32 ou32_InterfaceIndex,
                              const stw_types::uint32 ou32_ReceiveTimeout);

private:
   Ui::C_SdBueMessageRxEntry * mpc_Ui;
   QIcon mc_NodeActive;
   QIcon mc_NodeInactive;
   stw_types::uint32 mu32_NodeIndex;
   stw_types::uint32 mu32_InterfaceIndex;
   stw_types::uint32 mu32_LastKnownCycleTimeValue;
   bool mq_AlwaysHide;

   static const stw_types::sint32 mhs32_IndexAuto;
   static const stw_types::sint32 mhs32_IndexCustom;

   void m_OnCheckBoxToggled(void);
   void m_OnUseReceiveTimeoutChanged(void);
   void m_OnReceiveTimeoutChanged(void);
   void m_HandleInactiveStates(void) const;
   void m_UpdateAutoReceiveTimeoutValue(void) const;
   //Avoid call
   C_SdBueMessageRxEntry(const C_SdBueMessageRxEntry &);
   C_SdBueMessageRxEntry & operator =(const C_SdBueMessageRxEntry &);
};

/* -- Extern Global Variables ---------------------------------------------- */
} //end of namespace

#endif
