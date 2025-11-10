#pragma once
#include <functional>

// Define a Delegate class with a variable number of template arguments. 
// With this definition we can bind callback functions (lambdas) that can invoke the callbacks with a variable number of arguments (0 or more).
template<typename... Args>
class Delegate
{
public:
	Delegate() = default;
	~Delegate() = default;

	// Bind a callback function (lambda) that can accept a variable number of arguments.
	void Bind(std::function<void(Args...)> function)
	{
		Callback = std::move(function);
	}

	bool IsBound() const
	{
		return Callback != nullptr;
	}

	void Unbind()
	{
		if (Callback)
		{
			Callback = nullptr;
		}
	}

	void ExecuteIfBound(const Args... arguments) const
	{
		if (IsBound())
		{
			Callback(arguments...);
		}
	}

private:
	// Callback with return type void, and a variable number of arguments.
	std::function<void(Args...)> Callback;
};