#pragma once
#include <future>
#include <iostream>
namespace DataIO::Container
{
	template<typename Data_t, template<typename FutureType> typename Future_t=std::future >
	class AsyncContainer
	{
		using LpDataObject = std::shared_ptr<Data_t>;

		LpDataObject MakePointer(const Data_t& Input=Data_t{}) { return std::make_shared<Data_t>(Input); }

		// ”ñ“¯Šú‘—M
		std::unique_ptr<std::promise<LpDataObject>> Promise;
		// ”ñ“¯ŠúóM
		Future_t<LpDataObject> UniqueGetter;
		// ”ñ“¯ŠúóMŒ‹‰Ê‚ğ•Û‘¶
		std::shared_future<LpDataObject> SharedGetter;

		void InputUpdate(Data_t&& InputImage) noexcept;
		void OutputUpdate(const std::chrono::milliseconds timeout)   noexcept;
	public:
		AsyncContainer()noexcept;
		void Throw(Data_t&& InputImage) noexcept;
		bool Recieve(const std::chrono::milliseconds timeout, LpDataObject& GetImage) noexcept;
	};

	template<typename Data_t, template<typename FutureType> typename Future_t>
	inline AsyncContainer<Data_t, Future_t>::AsyncContainer() noexcept
	{

		this->Promise = std::make_unique<std::promise<LpDataObject>>();
		this->UniqueGetter = this->Promise->get_future();
		this->Promise->set_value(LpDataObject{});
	}
	template<typename Data_t, template<typename FutureType> typename Future_t>
	inline void AsyncContainer<Data_t, Future_t>::InputUpdate(Data_t&& InputImage) noexcept
	{
		if (!this->UniqueGetter.valid())
		{
			std::promise<LpDataObject> NewPromise;
			this->Promise->swap(NewPromise);
			this->UniqueGetter = this->Promise->get_future();
			this->Promise->set_value(MakePointer(InputImage));
		}
	}
	template<typename Data_t, template<typename FutureType> typename Future_t>
	inline void AsyncContainer<Data_t, Future_t>::Throw(Data_t&& InputImage) noexcept
	{
		try {
			InputUpdate(std::forward<Data_t>(InputImage));
		}
		catch (const std::future_error & e)
		{
			if (e.code() != std::future_errc::promise_already_satisfied)
			{
				ASSERT(0);
			}

		}
	}
	template<typename Data_t, template<typename FutureType> typename Future_t>
	inline void AsyncContainer<Data_t, Future_t>::OutputUpdate(const std::chrono::milliseconds timeout) noexcept
	{
		try {
			if (this->UniqueGetter.valid())
			{
				auto Result = this->UniqueGetter.wait_for(timeout);
				if (Result != std::future_status::timeout)
				{
					this->SharedGetter = this->UniqueGetter.share();
				}
			}
		}
		catch (const std::future_error & Err)
		{
			std_custum::string str;
			CodePoint::ConvertStringAuto(std::string(Err.what()), str);
			OutputDebugString(str.c_str());
		}
	}
	template<typename Data_t, template<typename FutureType> typename Future_t>
	[[nodiscard]]
	inline bool AsyncContainer<Data_t, Future_t>::Recieve(const std::chrono::milliseconds timeout, LpDataObject& GetData) noexcept
	{
		OutputUpdate(timeout);
		{
			if (this->SharedGetter.get())
			{
				GetData = (this->SharedGetter.get());
				return true;
			}
		}
		return false;
	}

}

