/**************************************************************************
*   Copyright (C) 2004-2007 by Michael Medin <michael@medin.name>         *
*                                                                         *
*   This code is part of NSClient++ - http://trac.nakednuns.org/nscp      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#pragma once

#include <NSCAPI.h>
#include <dll/dll.hpp>
#include <nsclient/logger.hpp>

/**
 * @ingroup NSClient++
 * Exception class for the NSCPlugin class.
 * When an unexpected error occurs in NSCPlugin this exception is thrown.
 *
 * @version 1.0
 * first version
 *
 * @date 02-12-2005
 *
 * @author mickem
 *
 * @par license
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 * 
 * @todo 
 * Add status codes to make error type simpler to parse out.
 *
 * @bug 
 *
 */
class NSPluginException : public std::exception {
	std::string file_;	// DLL filename (for which the exception was thrown)
	std::string error;
public:
	//////////////////////////////////////////////////////////////////////////
	/// Constructor takes an error message.
	/// @param error the error message
	///
	/// @author mickem
	NSPluginException(const std::wstring &module, const std::string &error) : file_(utf8::cvt<std::string>(module)), error(error) {}
	NSPluginException(const std::wstring &module, const std::wstring &error) : file_(utf8::cvt<std::string>(module)), error(utf8::cvt<std::string>(error)) {}
	~NSPluginException() throw() {}

	//////////////////////////////////////////////////////////////////////////
	/// Retrieve the error message from the exception.
	/// @return the error message
	///
	/// @author mickem
	const char* what() const throw() { return error.c_str(); }
	const std::string file() const throw() { return file_; }
	std::string reason() const throw() { return utf8::utf8_from_native(what()); }

};

/**
 * @ingroup NSClient++
 * NSCPlugin is a wrapper class to wrap all DLL calls and make things simple and clean inside the actual application.<br>
 * Things tend to be one-to-one by which I mean that a call to a function here should call the corresponding function in the plug in (if loaded).
 * If things are "broken" NSPluginException is called to indicate this. Error states are returned for normal "conditions".
 *
 *
 * @version 1.0
 * first version
 *
 * @date 02-12-2005
 *
 * @author mickem
 *
 * @par license
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 * 
 * @todo 
 * getVersion() is not implemented as of yet.
 *
 * @bug 
 *
 */
class NSCPlugin :  public boost::noncopyable, public nsclient::logging::raw_subscriber {
private:
	//bool bLoaded_;			// Status of plug in
	::dll::dll_impl module_;
	bool loaded_;
	bool broken_;
	unsigned int plugin_id_;
	std::wstring alias_;
	bool lastIsMsgPlugin_;

	nscapi::plugin_api::lpModuleHelperInit fModuleHelperInit;
	nscapi::plugin_api::lpLoadModule fLoadModule;
	nscapi::plugin_api::lpGetName fGetName;
	nscapi::plugin_api::lpGetVersion fGetVersion;
	nscapi::plugin_api::lpGetDescription fGetDescription;
	nscapi::plugin_api::lpHasCommandHandler fHasCommandHandler;
	nscapi::plugin_api::lpHasMessageHandler fHasMessageHandler;
	nscapi::plugin_api::lpHandleCommand fHandleCommand;
	nscapi::plugin_api::lpHandleMessage fHandleMessage;
	nscapi::plugin_api::lpDeleteBuffer fDeleteBuffer;
	nscapi::plugin_api::lpUnLoadModule fUnLoadModule;
	nscapi::plugin_api::lpCommandLineExec fCommandLineExec;
	nscapi::plugin_api::lpHasNotificationHandler fHasNotificationHandler;
	nscapi::plugin_api::lpHandleNotification fHandleNotification;
	nscapi::plugin_api::lpHasRoutingHandler fHasRoutingHandler;
	nscapi::plugin_api::lpRouteMessage fRouteMessage;

public:
	NSCPlugin(const unsigned int id, const boost::filesystem::path file, std::wstring alias);
	virtual ~NSCPlugin(void);

	std::string getSName() {
		return utf8::cvt<std::string>(getName());
	}

	std::wstring getName(void);
	std::wstring getDescription();
	void load_dll();
	bool load_plugin(NSCAPI::moduleLoadMode mode);
	void setBroken(bool broken);
	bool isBroken();
	bool getVersion(int *major, int *minor, int *revision);
	bool hasCommandHandler(void);
	bool hasNotificationHandler(void);
	bool hasMessageHandler(void);
	NSCAPI::nagiosReturn handleCommand(const wchar_t *command, const char* dataBuffer, const unsigned int dataBuffer_len, char** returnBuffer, unsigned int *returnBuffer_len);
	NSCAPI::nagiosReturn handleCommand(const wchar_t* command, std::string &request, std::string &reply);
	NSCAPI::nagiosReturn handleNotification(const wchar_t *channel, std::string &request, std::string &reply);
	NSCAPI::nagiosReturn handleNotification(const wchar_t *channel, const char* request_buffer, const unsigned int request_buffer_len, char** response_buffer, unsigned int *response_buffer_len);
	void deleteBuffer(char**buffer);
	void handleMessage(const char* data, unsigned int len);
	void unload_dll(void);
	void unload_plugin(void);
	std::wstring getCongifurationMeta();
	int commandLineExec(const wchar_t* command, std::string &request, std::string &reply);
	int commandLineExec(const wchar_t* command, const char* request, const unsigned int request_len, char** reply, unsigned int *reply_len);
	bool has_command_line_exec();
	bool is_duplicate( boost::filesystem::path file, std::wstring alias );


	bool has_routing_handler();

	
	bool route_message(const wchar_t *channel, const char* buffer, unsigned int buffer_len, wchar_t **new_channel_buffer, char **new_buffer, unsigned int *new_buffer_len);

	std::string get_description() {
		if (alias_.empty())
			return getSName();
		return getSName() + " (" + utf8::cvt<std::string>(alias_) + ")";
	}
	std::wstring get_alias() {
		return alias_;
	}
	inline std::wstring getFilename() const {
		return utf8::cvt<std::wstring>(module_.get_filename());
	}
	inline std::wstring get_alias_or_name() const {
		if (!alias_.empty())
			return alias_;
		return getFilename();
	}
	std::wstring getModule() {
#ifndef WIN32
		std::string file = module_.get_module_name();
		if (file.substr(0,3) == "lib")
			file = file.substr(3);
		return utf8::cvt<std::wstring>(file);
#else
		return utf8::cvt<std::wstring>(module_.get_module_name());
#endif
	}
	static std::wstring get_plugin_file(std::wstring key) {
#ifdef WIN32
		return key + _T(".dll");
#else
		return _T("lib") + key + _T(".so");
#endif
	}
	bool getLastIsMsgPlugin() {
		return lastIsMsgPlugin_;
	}
	bool isLoaded() const {
		return module_.is_loaded();
	}
	unsigned int get_id() const { return plugin_id_; }
	static boost::filesystem::path get_filename(boost::filesystem::path folder, std::wstring module);

public:
	void on_raw_log_message(std::string &payload) {
		handleMessage(payload.c_str(), payload.size());
	}

private:
	bool getName_(wchar_t* buf, unsigned int buflen);
	bool getDescription_(wchar_t* buf, unsigned int buflen);
	void loadRemoteProcs_(void);
};




