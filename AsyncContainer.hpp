#pragma once
#include <future>
#include <iostream>
namespace DataIO::Container
{
	template<typename Data_t>
	class AsyncContainer
	{
		using LpDataObject = std::shared_ptr<Data_t>;

		LpDataObject MakePointer(const Data_t& Input=Data_t{}) { return std::make_shared<Data_t>(Input); }

		// ”ñ“¯Šú‘—M
		std::unique_ptr<std::promise<LpDataObject>> m_Promise;
		// ”ñ“¯ŠúóM
		std::future<LpDataObject> m_UniqueFuture;
		// ”ñ“¯ŠúóMŒ‹‰Ê‚ğ•Û‘¶
		std::shared_future<LpDataObject> m_SharedFuture;

		void UpdateInput(Data_t&& InputImage) noexcept;

		template<typename DataType,typename Ratio>
		void UpdateOutput(const std::chrono::duration<DataType,Ratio>& Timeout)   noexcept;
	public:
		AsyncContainer()noexcept;
		void Throw(Data_t&& InputImage) noexcept;

		template<typename DataType,typename Ratio >
		bool Recieve(const std::chrono::duration<DataType,Ratio>& timeout, LpDataObject& GetImage) noexcept;

	};

	template<typename Data_t>
	inline AsyncContainer<Data_t>::AsyncContainer() noexcept
	{

		this->m_Promise = std::make_unique<std::promise<LpDataObject>>();
		this->m_UniqueFuture = this->m_Promise->get_future();
		this->m_Promise->set_value(LpDataObject{});
	}
	template<typename Data_t>
	inline void AsyncContainer<Data_t>::UpdateInput(Data_t&& InputImage) noexcept
	{
		if (!this->m_UniqueFuture.valid())
		{
			std::promise<LpDataObject> NewPromise;
			this->m_Promise->swap(NewPromise);
			this->m_UniqueFuture = this->m_Promise->get_future();
			this->m_Promise->set_value(MakePointer(InputImage));
		}
	}
	template<typename Data_t>
	inline void AsyncContainer<Data_t>::Throw(Data_t&& InputImage) noexcept
	{
		try {
			this->UpdateInput(std::forward<Data_t>(InputImage));
		}
		catch (const std::future_error & e)
		{
			if (e.code() != std::future_errc::promise_already_satisfied)
			{
				assert(0);
			}

		}
	}
	/*
	template<typename Data_t>
	inline void AsyncContainer<Data_t>::OutputUpdate(const std::chrono::milliseconds timeout) noexcept
	{
		try {
			if (this->m_UniqueFuture.valid())
			{
				auto Result = this->m_UniqueFuture.wait_for(timeout);
				if (Result != std::future_status::timeout)
				{
					this->m_SharedFuture = this->m_UniqueFuture.share();
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
	template<typename Data_t>
	[[nodiscard]]
	inline bool AsyncContainer<Data_t>::Recieve(const std::chrono::milliseconds timeout, LpDataObject& GetData) noexcept
	{
		OutputUpdate(timeout);

		if (this->m_SharedFuture.get())
		{
			GetData = (this->m_SharedFuture.get());
			return true;
		}

		return false;
	}*/
	template<typename Data_t>
	template<typename DataType, typename Ratio>
	inline void AsyncContainer<Data_t>::UpdateOutput(const std::chrono::duration<DataType, Ratio>& Timeout) noexcept
	{

		try {
			if (this->m_UniqueFuture.valid())
			{
				auto Result = this->m_UniqueFuture.wait_for(Timeout);
				if (Result != std::future_status::timeout)
				{
					this->m_SharedFuture = this->m_UniqueFuture.share();
				}
			}
		}
		catch (const std::future_error& Err)
		{
			std_custum::string str;
			CodePoint::ConvertStringAuto(std::string(Err.what()), str);
			OutputDebugString(str.c_str());
		}
	}
	template<typename Data_t>
	template<typename DataType,typename Ratio >
	[[nodiscard]] inline bool AsyncContainer<Data_t>::Recieve(const std::chrono::duration<DataType, Ratio>& timeout, LpDataObject& GetImage) noexcept
	{
		this->UpdateOutput(timeout);

		if (this->m_SharedFuture.get())
		{
			GetData = (this->m_SharedFuture.get());
			return true;
		}

		return false;

	}



}

