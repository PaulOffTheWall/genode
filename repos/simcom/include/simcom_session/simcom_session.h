#ifndef _INCLUDE__SIMCOM_SESSION__SIMCOM_SESSION_H_
#define _INCLUDE__SIMCOM_SESSION__SIMCOM_SESSION_H_

#include <session/session.h>
#include <base/rpc.h>
#include <simcom/types.h>

namespace SimCom {

	struct Session : Genode::Session
	{
		static const char *service_name() { return "SimCom"; }

		virtual SensorDataOut get_sensor_data() = 0;
		virtual void set_command_data(SensorDataIn commandData) = 0;


		/*******************
		 ** RPC interface **
		 *******************/
		GENODE_RPC(Rpc_get_sensor_data, SensorDataOut, get_sensor_data);
		GENODE_RPC(Rpc_set_command_data, void, set_command_data, SensorDataIn);

		GENODE_RPC_INTERFACE(Rpc_get_sensor_data, Rpc_set_command_data);
	};
}

#endif /* _INCLUDE__SIMCOM_SESSION__SIMCOM_SESSION_H_ */
