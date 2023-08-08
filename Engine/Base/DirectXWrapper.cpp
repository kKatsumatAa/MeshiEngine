#include "DirectXWrapper.h"
#include <thread>
#include <d3dx12.h>
#include "Util.h"

using namespace DirectX;
using namespace Microsoft::WRL;


void DirectXWrapper::InitializeDevice()
{
	// DXGI�t�@�N�g���[�̐���
	result_ = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result_));
	// �A�_�v�^�[�̗񋓗p
	std::vector< ComPtr<IDXGIAdapter4>> adapters;
	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	ComPtr < IDXGIAdapter4> tmpAdapter = nullptr;
	// �p�t�H�[�}���X���������̂��珇�ɁA�S�ẴA�_�v�^�[��񋓂���
	for (uint32_t i = 0;
		dxgiFactory_->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++) {
		// ���I�z��ɒǉ�����
		adapters.push_back(tmpAdapter);
	}

	// �Ó��ȃA�_�v�^��I�ʂ���
	for (int32_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;
		// �A�_�v�^�[�̏����擾����
		adapters[i]->GetDesc3(&adapterDesc);
		// �\�t�g�E�F�A�f�o�C�X�����
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// �f�o�C�X���̗p���ă��[�v�𔲂���
			tmpAdapter = adapters[i].Get();
			break;
		}
	}

	// �Ή����x���̔z��
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (int32_t i = 0; i < _countof(levels); i++) {
		// �̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result_ = D3D12CreateDevice(tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device_));
		if (result_ == S_OK) {
			// �f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
			break;
		}
	}
}

void DirectXWrapper::InitializeCommand()
{
	// �`��R�}���h�n�𐶐�
	CreateCommandLA(commandAllocator_.GetAddressOf(), commandList_.GetAddressOf());

	//�e�N�X�`���̃R�}���h�n
	CreateCommandLA(texCommandAllocator_.GetAddressOf(), texCommandList_.GetAddressOf());

	//�R�}���h�L���[�̐ݒ�
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//�R�}���h�L���[�𐶐�
	result_ = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(result_));
	//�e�N�X�`���p��
	result_ = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&texCommandQueue_));
	assert(SUCCEEDED(result_));
}

void DirectXWrapper::InitializeSwapchain()
{
	//�X���b�v�`�F�[���ݒ�
	swapChainDesc_.Width = (uint32_t)WindowsApp::GetInstance().WINDOW_WIDTH_;
	swapChainDesc_.Height = (uint32_t)WindowsApp::GetInstance().WINDOW_HEIGHT_;
	swapChainDesc_.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // �F���̏���
	swapChainDesc_.SampleDesc.Count = 1; // �}���`�T���v�����Ȃ�
	swapChainDesc_.BufferUsage = DXGI_USAGE_BACK_BUFFER; // �o�b�N�o�b�t�@�p
	swapChainDesc_.BufferCount = 2; // �o�b�t�@����2�ɐݒ�
	swapChainDesc_.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // �t���b�v��͔j��
	swapChainDesc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// �X���b�v�`�F�[���̐���
	ComPtr<IDXGISwapChain1> swapChain1;

	result_ = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(), WindowsApp::GetInstance().Gethwnd(), &swapChainDesc_, nullptr, nullptr,
		&swapChain1);
	assert(SUCCEEDED(result_));

	swapChain1.As(&swapChain_);
}

void DirectXWrapper::InitializeRendertargetView()
{
	//�f�X�N���v�^�q�[�v�̐ݒ�
	rtvHeapDesc_.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // �����_�[�^�[�Q�b�g�r���[
	rtvHeapDesc_.NumDescriptors = swapChainDesc_.BufferCount; // ���\��2��
	// �f�X�N���v�^�q�[�v�̐���
	device_->CreateDescriptorHeap(&rtvHeapDesc_, IID_PPV_ARGS(rtvHeap_.GetAddressOf()));

	//�o�b�N�o�b�t�@
	backBuffers_.resize(swapChainDesc_.BufferCount);

	//RTV����
		// �X���b�v�`�F�[���̑S�Ẵo�b�t�@�ɂ��ď�������
	for (int32_t i = 0; i < backBuffers_.size(); i++) {
		// �X���b�v�`�F�[������o�b�t�@���擾
		swapChain_->GetBuffer((uint32_t)i, IID_PPV_ARGS(&backBuffers_[i]));
		// �f�X�N���v�^�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
		// �����\���ŃA�h���X�������
		rtvHandle.ptr += i * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);
		// �����_�[�^�[�Q�b�g�r���[�̐ݒ�
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		// �V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		// �����_�[�^�[�Q�b�g�r���[�̐���
		device_->CreateRenderTargetView(backBuffers_[i].Get(), &rtvDesc, rtvHandle);
	}


}

void DirectXWrapper::InitializeDepthBuffer()
{
	//06_01
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = (uint16_t)WindowsApp::GetInstance().WINDOW_WIDTH_;//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResourceDesc.Height = (uint32_t)WindowsApp::GetInstance().WINDOW_HEIGHT_;//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_R32_TYPELESS;//�[�x�l�t�H�[�}�b�g(�o�b�t�@�[�Ƃ��Ẵr�b�g����32�����r�b�g���ȊO�̈����͍ŏI�I�Ɍ��߂���)
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//�f�v�X�X�e���V��
	//�[�x�l�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//�[�x�l�N���A�ݒ�
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//�[�x�l1.0f(�ő�l)�ŃN���A
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//�[�x�l�t�H�[�}�b�g
	//���\�[�X����
	result_ = device_->CreateCommittedResource(
		&depthHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff_));
	assert(SUCCEEDED(result_));
	//�[�x�r���[�p�f�X�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;//�[�x�r���[�͈��
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//�f�v�X�X�e���V���r���[
	result_ = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(dsvHeap_.GetAddressOf()));
	assert(SUCCEEDED(result_));

	//�[�x�r���[�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//�[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device_->CreateDepthStencilView(
		depthBuff_.Get(),
		&dsvDesc,//�[�x�r���[
		dsvHeap_->GetCPUDescriptorHandleForHeapStart()//�q�[�v�̐擪�ɍ��
	);

	//---------

	//�[�x�l���e�N�X�`���Ƃ��Ďg�����߂�SRV�̃q�[�v�쐬
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	dsvHeapDesc.NodeMask = 0;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	result_ = device_->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&depthSRVHeap_));
	assert(SUCCEEDED(result_));

	//srv�쐬
	D3D12_SHADER_RESOURCE_VIEW_DESC resDesc = {};
	resDesc.Format = DXGI_FORMAT_R32_FLOAT;
	resDesc.Texture2D.MipLevels = 1;
	resDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	resDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;

	device_->CreateShaderResourceView(depthBuff_.Get(), &resDesc,
		depthSRVHeap_->GetCPUDescriptorHandleForHeapStart());
}

void DirectXWrapper::InitializeFence()
{
	//�t�F���X����
	result_ = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));

	result_ = device_->CreateFence(texFenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&texFence_));
}

void DirectXWrapper::InitializeFixFPS()
{
	//���ݎ��Ԃ��L�^
	reference_ = std::chrono::steady_clock::now();
}

void DirectXWrapper::UpdateFixFPS()
{
	//1/60(s)�҂�����̎���
	const std::chrono::microseconds MIN_TIME(uint64_t(1000000.0f / 60.0f));
	//1/60(s)���킸���ɒZ������
	const std::chrono::microseconds MIN_CHECK_TIME(uint64_t(1000000.0f / 65.0f));

	//���ݎ��Ԃ��擾
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	//1/60(s)�i���킸���ɒZ�����ԁj�o���ĂȂ��ꍇ
	if (elapsed < MIN_CHECK_TIME)
	{
		//1/60(s)�o�߂���܂Ŕ����ȃX���[�v�J��Ԃ�
		while (std::chrono::steady_clock::now() - reference_ < MIN_CHECK_TIME)//
		{
			//1�}�C�N���b�X���[�v
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	//���݂̎��Ԃ��L�^
	reference_ = std::chrono::steady_clock::now();
}

void DirectXWrapper::CreateCommandLA(ID3D12CommandAllocator** texCommandAllocator, ID3D12GraphicsCommandList** texCommandList)
{
	// �R�}���h�A���P�[�^�𐶐�
	result_ = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(texCommandAllocator));
	assert(SUCCEEDED(result_));
	// �R�}���h���X�g�𐶐�
	result_ = device_->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		(*texCommandAllocator), nullptr,
		IID_PPV_ARGS(texCommandList));
	assert(SUCCEEDED(result_));
}

void DirectXWrapper::Initialize()
{
	//FPS�Œ菉����
	InitializeFixFPS();

	//�f�o�C�X�����iifdef�n���֐��ɂ���Ɖ��̂��G���[���N����j
#ifdef _DEBUG
		//�f�o�b�O���C���[���I����
	ComPtr < ID3D12Debug1> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf())))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(true);//�d���̂ŕ`�悪�ςɂȂ������̂�
	}
#endif
	InitializeDevice();
#ifdef _DEBUG
	ComPtr < ID3D12InfoQueue> infoQueue;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf()))))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);//��΂��G���[���~�܂�
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);//�G���[���~�܂�
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);//�x�����~�܂�
		infoQueue->Release();
	}

	//�}������G���[
	D3D12_MESSAGE_ID denyIds[] = {
		//win11�ł̃o�O�ɂ��G���[���b�Z
		D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
	};
	//�}������\�����x��
	D3D12_MESSAGE_SEVERITY serverities[] = { D3D12_MESSAGE_SEVERITY_INFO };
	D3D12_INFO_QUEUE_FILTER filter{};
	filter.DenyList.NumIDs = _countof(denyIds);
	filter.DenyList.pIDList = denyIds;
	filter.DenyList.NumSeverities = _countof(serverities);
	filter.DenyList.pSeverityList = serverities;
	//�w�肵���G���[�̕\����}��
	infoQueue->PushStorageFilter(&filter);

#endif 

	//�R�}���h�֘A
	InitializeCommand();
	//�X���b�v�`�F�[��
	InitializeSwapchain();
	//�����_�[�^�[�Q�b�g
	InitializeRendertargetView();
	//�[�x�o�b�t�@
	InitializeDepthBuffer();
	//�t�F���X
	InitializeFence();
}

DirectXWrapper::DirectXWrapper()
{
}

DirectXWrapper::~DirectXWrapper()
{
}



DirectXWrapper& DirectXWrapper::GetInstance()
{
	static DirectXWrapper sInst; // private �ȃR���X�g���N�^���Ăяo���B
	return sInst;
}

void DirectXWrapper::CommandReset()
{
	// �L���[���N���A
	result_ = commandAllocator_->Reset();
	assert(SUCCEEDED(result_));
	// �ĂуR�}���h���X�g�𒙂߂鏀��
	result_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result_));
}

void DirectXWrapper::DrawInitialize()
{

}

void DirectXWrapper::UpLoadTexture()
{
	// ���߂̃N���[�Y
#pragma region CmdClose
	HRESULT result = texCommandList_->Close();
	assert(SUCCEEDED(result));
	// ���߂Ă����R�}���h���X�g�̎��s(close�K�{)
	ID3D12CommandList* commandLists[] = { texCommandList_.Get() };
	texCommandQueue_->ExecuteCommandLists(1, commandLists);
#pragma endregion CmdClose

#pragma region ChangeScreen
	// �R�}���h�̎��s������҂�
	texCommandQueue_->Signal(texFence_.Get(), ++texFenceVal_);
	if (texFence_->GetCompletedValue() != texFenceVal_)	//	GPU�̏�������������������
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		texFence_->SetEventOnCompletion(texFenceVal_, event);
		if (event != 0) {
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
	}
	// �L���[���N���A
	result = texCommandAllocator_->Reset();
	assert(SUCCEEDED(result));
	// �ĂуR�}���h���X�g�𒙂߂鏀��
	result = texCommandList_->Reset(texCommandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(result));
	texUploadBuff_.clear();
}

//���ۂɕ`���pre
void DirectXWrapper::PreDraw()
{
	// �o�b�N�o�b�t�@�̔ԍ����擾(2�Ȃ̂�0�Ԃ�1��)
	uint32_t bbIndex = swapChain_->GetCurrentBackBufferIndex();

	rtvHandle_ = rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
	rtvHandle_.ptr += bbIndex * device_->GetDescriptorHandleIncrementSize(rtvHeapDesc_.Type);

	//1.���\�[�X�o���A�ŏ������݉\�ɕύX
	barrierDesc_.Transition.pResource = backBuffers_[bbIndex].Get(); // �o�b�N�o�b�t�@���w��
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ���
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ�
	commandList_->ResourceBarrier(1, &barrierDesc_);

	//�����_�[�^�[�Q�b�g(�|�X�g�y���̈ꖇ�ڂ�dsv�ƕR�Â���̂ł����ł͂Ȃ�)
	commandList_->OMSetRenderTargets(
		1, &rtvHandle_, false, nullptr
	);

	// 3.��ʃN���A R G B A
	commandList_->ClearRenderTargetView(rtvHandle_, clearColor_, 0, nullptr);

	// �r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	commandList_->RSSetViewports(1, &WindowsApp::GetInstance().viewport_);

	// �V�U�[��`
	D3D12_RECT scissorRect{};
	scissorRect.left = 0; // �؂蔲�����W��
	scissorRect.right = (long)(scissorRect.left + WindowsApp::GetInstance().WINDOW_WIDTH_); // �؂蔲�����W�E
	scissorRect.top = 0; // �؂蔲�����W��
	scissorRect.bottom = (long)(scissorRect.top + WindowsApp::GetInstance().WINDOW_HEIGHT_); // �؂蔲�����W��
	// �V�U�[��`�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
	commandList_->RSSetScissorRects(1, &scissorRect);
}

void DirectXWrapper::PostDraw()
{
	barrierDesc_.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET; // �`���Ԃ���
	barrierDesc_.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT; // �\����Ԃ�
	commandList_->ResourceBarrier(1, &barrierDesc_);

	// ���߂̃N���[�Y
	result_ = commandList_->Close();
	assert(SUCCEEDED(result_));
	// �R�}���h���X�g�̎��s
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);

	// ��ʂɕ\������o�b�t�@���t���b�v(���\�̓��ւ�)
	result_ = swapChain_->Present(1, 0);

	// �R�}���h�̎��s������҂�
	commandQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_)//GPU�̏���������������
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceVal_, event);
		if (event != 0) {
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
	}

	//FPS�Œ�
	UpdateFixFPS();

	//���Z�b�g
	CommandReset();

	assert(SUCCEEDED(result_));
}

void DirectXWrapper::ResourceBarrier(D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState, ID3D12Resource* buff)
{
	D3D12_RESOURCE_BARRIER barrierDesc{};

	barrierDesc.Transition.pResource = buff;
	barrierDesc.Transition.StateBefore = beforeState; // �`���Ԃ���
	barrierDesc.Transition.StateAfter = afterState; // �\����Ԃ�
	commandList_->ResourceBarrier(1, &barrierDesc);
}

void LoadPictureFromFile(const char* fileName, ComPtr<ID3D12Resource>& texBuff)
{
	HRESULT result = {};

	wchar_t fileNameL[128];
	ConstCharToWcharT(fileName, fileNameL);

	// 04_03
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WIC�̃e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		fileNameL,
		WIC_FLAGS_NONE,
		&metadata, scratchImg
	);

	assert(SUCCEEDED(result));

	ScratchImage mipChain{};
	//mipmap����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result))
	{
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);

	//�R�s�[��p
	// �q�[�v�̐ݒ�
	CD3DX12_HEAP_PROPERTIES textureHeapProp =
		CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC textureResourceDesc =
		CD3DX12_RESOURCE_DESC::Tex2D(
			metadata.format,
			(uint64_t)metadata.width,
			(uint32_t)metadata.height,
			(uint16_t)metadata.arraySize,
			(uint16_t)metadata.mipLevels,
			1);

	// �e�N�X�`���o�b�t�@�̐���
	result = DirectXWrapper::GetInstance().GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(texBuff.GetAddressOf()));
	assert(SUCCEEDED(result));


	//�A�b�v���[�h�o�b�t�@
	// �q�[�v�̐ݒ�
	uint64_t uploadSize = GetRequiredIntermediateSize(texBuff.Get(), 0, (uint32_t)metadata.mipLevels);

	D3D12_HEAP_PROPERTIES uploadHeapProp{};
	uploadHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC resDesc =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuff;

	// �e�N�X�`���o�b�t�@�̐���
	result = DirectXWrapper::GetInstance().GetDevice()->
		CreateCommittedResource(
			&uploadHeapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuff));
	assert(SUCCEEDED(result));


	//�T�u���\�[�X����
	std::vector<D3D12_SUBRESOURCE_DATA> subResourcesDatas{};
	subResourcesDatas.resize(metadata.mipLevels);

	for (int32_t i = 0; i < subResourcesDatas.size(); i++)
	{
		// �S�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* IMG = scratchImg.GetImage(i, 0, 0);

		subResourcesDatas[i].pData = IMG->pixels;
		subResourcesDatas[i].RowPitch = IMG->rowPitch;
		subResourcesDatas[i].SlicePitch = IMG->slicePitch;
	}

	//�T�u���\�[�X��]��
	UpdateSubresources(
		DirectXWrapper::GetInstance().GetCommandList(),
		texBuff.Get(),
		uploadBuff.Get(),
		0,
		0,
		(uint32_t)metadata.mipLevels,
		subResourcesDatas.data()
	);

	//���f�[�^���
	scratchImg.Release();

	//�o���A�ƃt�F���X
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	BarrierDesc.Transition.pResource = texBuff.Get();
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;//�������d�v
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;//�������d�v

	DirectXWrapper::GetInstance().GetCommandList()->ResourceBarrier(1, &BarrierDesc);
	DirectXWrapper::GetInstance().GetCommandList()->Close();

	//�R�}���h���X�g�̎��s
	ID3D12CommandList* cmdlists[] = { DirectXWrapper::GetInstance().GetCommandList() };
	DirectXWrapper::GetInstance().GetCommandQueue()->ExecuteCommandLists(1, cmdlists);

	//�R�}���h����
	DirectXWrapper::GetInstance().GetCommandQueue()->Signal(DirectXWrapper::GetInstance().GetFence(),
		++DirectXWrapper::GetInstance().GetFenceVal());

	if (DirectXWrapper::GetInstance().GetFence()->GetCompletedValue() != DirectXWrapper::GetInstance().GetFenceVal())
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		DirectXWrapper::GetInstance().GetFence()->SetEventOnCompletion(DirectXWrapper::GetInstance().GetFenceVal(), event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	//�o�b�t�@���
	/*uploadBuff->Release();
	uploadBuff.Reset();*/

	//�R�}���h���Z�b�g
	DirectXWrapper::GetInstance().CommandReset();
}

//------------------------------------------------------------------------------
void ResourceProperties(D3D12_RESOURCE_DESC& resDesc, uint32_t size)
{
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = size;						//���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
}

void BuffProperties(D3D12_HEAP_PROPERTIES& heap, D3D12_RESOURCE_DESC& resource, ID3D12Resource** buff)
{
	HRESULT result = {};

	result = DirectXWrapper::GetInstance().GetDevice()->CreateCommittedResource(
		&heap,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resource,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(buff));
	assert(SUCCEEDED(result));
}


