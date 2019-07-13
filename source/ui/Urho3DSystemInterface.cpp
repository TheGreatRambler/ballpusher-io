#include "Urho3DSystemInterface.h"
#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/Timer.h>
#include <Urho3D/IO/Log.h>

// Get the number of seconds elapsed since the start of the application
double Urho3DSystemInterface::GetElapsedTime() {
	Urho3D::Time* time = this->context->GetSubsystem<Urho3D::Time>();
	return time->GetElapsedTime();
}

bool LogMessage(Rml::Core::Log::Type type, const Rml::Core::String& message) {
	// We don't care about the type for now
	URHO3D_LOGINFO(message.CString());
}