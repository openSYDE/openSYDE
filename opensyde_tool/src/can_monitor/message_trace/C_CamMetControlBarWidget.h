//----------------------------------------------------------------------------------------------------------------------
/*!
   \file
   \brief       Widget with all configuration and controlling elements for the trace
 (header)

   See cpp file for detailed description

   \copyright   Copyright 2018 Sensor-Technik Wiedemann GmbH. All rights reserved.
*/
//----------------------------------------------------------------------------------------------------------------------
#ifndef C_CAMMETCONTROLBARWIDGET_H
#define C_CAMMETCONTROLBARWIDGET_H

/* -- Includes ------------------------------------------------------------------------------------------------------ */
#include <QWidget>

#include "stwtypes.h"

#include "CCMONProtocol.h"

/* -- Namespace ----------------------------------------------------------------------------------------------------- */
namespace Ui
{
class C_CamMetControlBarWidget;
}

namespace stw_opensyde_gui
{
/* -- Global Constants ---------------------------------------------------------------------------------------------- */

/* -- Types --------------------------------------------------------------------------------------------------------- */

class C_CamMetControlBarWidget :
   public QWidget
{
   Q_OBJECT

public:
   explicit C_CamMetControlBarWidget(QWidget * const opc_Parent = NULL);
   ~C_CamMetControlBarWidget(void);

   void InitStaticNames(void) const;

   void LoadUserSettings(void);
   void SaveUserSettings(void) const;

   void StopLogging(void);

   void SearchBarFocus(void);
   void SearchNext(void);
   void SearchPrev(void);

   //The signals keyword is necessary for Qt signal slot functionality
   //lint -save -e1736

Q_SIGNALS:
   void SigStartLogging(void);
   void SigPauseLogging(void);
   void SigContinueLogging(void);
   void SigStopLogging(void);

   void SigClearData(void);
   void SigDisplayAsHex(const bool oq_Value);
   void SigDisplayTimestampRelative(const bool oq_Value);
   void SigDisplayTimestampTimeOfDay(const bool oq_Value);
   void SigTraceBufferSize(const stw_types::uint32 ou32_Value);
   void SigChangeProtocol(const stw_cmon_protocol::e_CMONL7Protocols oe_Protocol);
   void SigDisplayTree(const bool oq_Show);
   void SigDisplayUniqueMessages(const bool oq_Unique);
   void SigSearchTrace(const QString & orc_SearchString, const bool oq_Next) const;
   //lint -restore

private:
   Ui::C_CamMetControlBarWidget * mpc_Ui;

   // Trace settings
   bool mq_DisplayTimestampAbsoluteTimeOfDay;
   stw_types::uint32 mu32_TraceBufferSize;

   static const stw_types::sintn mhsn_MAX_COUNT_COMBO_BOX;

   //Avoid call
   C_CamMetControlBarWidget(const C_CamMetControlBarWidget &);
   C_CamMetControlBarWidget & operator =(const C_CamMetControlBarWidget &);

   void m_HandleTogglePlay(const bool oq_Playing);
   void m_HandleStop(void);

   void m_OnProtocolIndexChange(const stw_types::sintn osn_Index);
   static stw_cmon_protocol::e_CMONL7Protocols mh_GetProtocolFromIndex(const stw_types::sint32 os32_Protocol);

   void m_HandleTreeVisibility(void);
   void m_HandleToggleDisplayMode(const bool oq_Active);

   void m_SearchComboBoxChanged(void) const;
   void m_UpdateSearchComboBox(void) const;

   void m_OpenTraceSettings(void);

   void m_MessageSearchWhileTracing(void);
};

/* -- Extern Global Variables --------------------------------------------------------------------------------------- */
} //end of namespace

#endif
