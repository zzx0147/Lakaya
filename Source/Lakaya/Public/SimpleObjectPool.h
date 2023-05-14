#pragma once
#include "functional"
#include "list"

template <class T>
class TSimpleObjectPool
{
public:
	/**
	 * @brief 오브젝트 풀을 생성합니다.
	 * @param InitialCount 초기에 생성할 오브젝트 개수입니다.
	 * @param MakeFunction 오브젝트 생성에 사용되는 함수입니다. 초기에 사용될 뿐만아니라, 더 이상 풀에 남은 오브젝트가 없는 경우에도 사용됩니다. 
	 */
	void SetupObjectPool(const uint16& InitialCount, std::function<T*()> MakeFunction);

	// 풀에서 오브젝트를 가져옵니다. 풀이 빈 경우, 오브젝트를 생성하여 반환합니다.
	T* GetObject();

	/**
	 * @brief 오브젝트를 다시 풀에 넣습니다.
	 * @param Object 반환할 오브젝트입니다.
	 */
	void ReturnObject(T* Object);

private:
	std::list<T*> AvailableObjects;
	std::function<T*()> InstanceMaker = nullptr;
};

template <class T>
void TSimpleObjectPool<T>::SetupObjectPool(const uint16& InitialCount, std::function<T*()> MakeFunction)
{
	InstanceMaker = MakeFunction;
	for (uint16 Count = 0; Count < InitialCount; ++Count) AvailableObjects.emplace_back(InstanceMaker());
}

template <class T>
T* TSimpleObjectPool<T>::GetObject()
{
	if (AvailableObjects.empty())
	{
		if (InstanceMaker) return InstanceMaker();
		UE_LOG(LogTemp, Error, TEXT("Pool was not setted!"))
		return nullptr;
	}

	auto Instance = AvailableObjects.back();
	AvailableObjects.pop_back();
	return Instance;
}

template <class T>
void TSimpleObjectPool<T>::ReturnObject(T* Object)
{
	AvailableObjects.emplace_back(Object);
}
