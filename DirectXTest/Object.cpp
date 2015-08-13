
#include "Builder.h"

void Vertex_t::SetPos (float x_,
					   float y_,
					   float z_)
{
	x = x_;
	y = y_;
	z = z_;
}

void Vertex_t::SetNormal (float nx_,
					      float ny_,
					      float nz_)
{
	nx = nx_;
	ny = ny_;
	nz = nz_;
}

void Vertex_t::SetTexture (float u_,
						   float v_,
						   float w_)
{
	u = u_;
	v = v_;
	w = w_;
}

void Vertex_t::SetColor (float r_,
						 float g_,
						 float b_,
						 float a_)
{
	r = r_;
	g = g_;
	b = b_;
	a = a_;
}

void Direct3DObject::ok ()
{
	DEFAULT_OK_BLOCK
}

Direct3DObject::Direct3DObject (XMMATRIX& world,
								bool blending,
								bool drawIndexed,
								D3D11_PRIMITIVE_TOPOLOGY topology) :
	vertices_     (),
	indices_      (),
	topology_     (topology),
	drawIndexed_  (drawIndexed),
	objectId_     (),
	blending_     (blending),
	objData_      ({ XMMatrixIdentity (), XMMatrixIdentity () }),
	world_        (world),
	vertexBuffer_ (nullptr),
	indexBuffer_  (nullptr),
	objectBuffer_ (nullptr),
	buffersSet_   (false),
	vertexShader_ ({ SHADER_NOT_SET }),
	pixelShader_  ({ SHADER_NOT_SET }),
	layoutN_      (0)
{

}

Direct3DObject::~Direct3DObject ()
{
	vertices_.clear ();
	indices_.clear ();
	objectId_ = 0;
	if (buffersSet_)
	{
		vertexBuffer_->Release ();
		vertexBuffer_ = nullptr;
		if (drawIndexed_)
		{
			indexBuffer_->Release ();
			indexBuffer_ = nullptr;
		}
		objectBuffer_->Release ();
		objectBuffer_ = nullptr;
	}
}

void Direct3DObject::SetID (UINT objectId)
{
	BEGIN_EXCEPTION_HANDLING
	objectId_ = objectId;
	END_EXCEPTION_HANDLING (SET_ID)
}

void Direct3DObject::AddVertexArray (Vertex_t* vert, size_t n)
{
	BEGIN_EXCEPTION_HANDLING
		if (buffersSet_)
			_EXC_N (BUFFERS_SET, "D3D: Cannot add vertices to buffered object")
			//vertices_.insert (vertices_.begin (), vert, vert + n);
			for (int i = 0; i < n; i++) vertices_.push_back (vert [i]);
	END_EXCEPTION_HANDLING (ADD_VERTEX_ARRAY)
}

void Direct3DObject::AddIndexArray (UINT* ind, size_t n)
{
	BEGIN_EXCEPTION_HANDLING
	if (buffersSet_)
		_EXC_N (BUFFERS_SET, "D3D: Cannot add indices to buffered object")
	indices_.insert (indices_.begin (), ind, ind + n);
	END_EXCEPTION_HANDLING (ADD_INDEX_ARRAY)
}

void Direct3DObject::SetupBuffers (ID3D11Device* device)
{
	BEGIN_EXCEPTION_HANDLING

	HRESULT result = S_OK;

	D3D11_BUFFER_DESC bufferDesc = {};

	if (vertices_.size () == 0)
		_EXC_N (EMPTY_VERTICES, "D3D: Cannot create empty vertex buffer (obj %d)" _ objectId_)

	if (drawIndexed_ && indices_.size () != 0)
	{
		bufferDesc = {};

		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = indices_.size () * sizeof (UINT);
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA indexSd = { indices_.data () };

		result = device->CreateBuffer (&bufferDesc,
									   &indexSd,
									   &indexBuffer_);
		if (result != S_OK)
			_EXC_N (INDEX_BUFFER, "D3D: Failed to create index buffer (obj %d)" _ objectId_)
	}
	if (drawIndexed_ && indices_.size () == 0)
		_EXC_N (EMPTY_INDICES, "D3D: Cannot create empty index buffer (obj %d)" _ objectId_)

	bufferDesc = {};

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof (Vertex_t) * vertices_.size ();
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA vertexSd = { vertices_.data () };

	result = device->CreateBuffer (&bufferDesc, 
								   &vertexSd, 
								   &vertexBuffer_);

	if (result != S_OK)
		_EXC_N (VERTEX_BUFFER, "D3D: Failed to create vertex buffer (obj %d)" _ objectId_)


	bufferDesc = {};

	bufferDesc.Usage           = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth       = sizeof (Direct3DObjectBuffer);
	bufferDesc.BindFlags       = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags  = 0;
	bufferDesc.MiscFlags       = 0;

	result = device->CreateBuffer (&bufferDesc, 
								   NULL, 
								   &objectBuffer_);
	
	if (result != S_OK)
		_EXC_N (OBJECT_BUFFER, "D3D: Failed to create object buffer (obj %d)" _ objectId_)

	buffersSet_ = true;

	END_EXCEPTION_HANDLING (SETUP_BUFFERS)
}

void Direct3DObject::SetWVP (XMMATRIX& matrix)
{
	objData_.WVP = matrix;
}

void Direct3DObject::SetWorld (XMMATRIX& matrix)
{
	objData_.World = matrix;
}

XMMATRIX& Direct3DObject::GetWorld ()
{
	return world_;
}

void Direct3DObject::Draw (ID3D11DeviceContext* deviceContext, 
						   Direct3DCamera* cam)
{
	BEGIN_EXCEPTION_HANDLING

	if (buffersSet_ == false)
		_EXC_N (BUFFERS_NOT_SET, 
				"D3D: Unable to draw with the buffers not set (obj %d)" _ 
				objectId_)

	if (vertices_.size () == 0)
		_EXC_N (EMPTY_VERTICES, 
				"D3D: Unable to draw object with empty vertex buffer (obj %d)" _ 
				objectId_)

	UINT stride = sizeof (Vertex_t);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers (0, 1, &vertexBuffer_, &stride, &offset);
	if (drawIndexed_)
	{
		if (indices_.size () == 0)
			_EXC_N (EMPTY_INDICES,
					"D3D: Unable to draw object with empty index buffer (obj %d)" _
					objectId_)
			deviceContext->IASetIndexBuffer (indexBuffer_, DXGI_FORMAT_R32_UINT, 0);
	}
	XMMATRIX tempWVP = world_ * cam->GetView () * cam->GetProjection ();

	objData_.WVP   = XMMatrixTranspose (tempWVP);
	//objData_.World = XMMatrixTranspose (world_);


	deviceContext->UpdateSubresource (objectBuffer_, 
									  0, 
									  NULL, 
									  &objData_, 
									  0, 
									  0);
	deviceContext->VSSetConstantBuffers (0,
										 1,
										 &objectBuffer_);

	deviceContext->IASetPrimitiveTopology (topology_);

	if (drawIndexed_)
		deviceContext->DrawIndexed (indices_.size (), 0, 0);
	else
		deviceContext->Draw (vertices_.size (), 0);

	END_EXCEPTION_HANDLING (DRAW_OBJECT)

}


void Direct3DObject::AttachVertexShader (ShaderDesc_t desc)
{
	BEGIN_EXCEPTION_HANDLING
		if (desc.type != SHADER_VERTEX)
			_EXC_N (SHADER_TYPE, "D3D: Invalid shader type, cannot use as vertex shader (obj %d)" _
					objectId_)
			vertexShader_ = desc;
	END_EXCEPTION_HANDLING (ATTACH_VERTEX_SHADER)
}

void Direct3DObject::AttachPixelShader (ShaderDesc_t desc)
{
	BEGIN_EXCEPTION_HANDLING
		if (desc.type != SHADER_PIXEL)
			_EXC_N (SHADER_TYPE, "D3D: Invalid shader type, cannot use as pixel shader (obj %d)" _
					objectId_)
			pixelShader_ = desc;
	END_EXCEPTION_HANDLING (ATTACH_PIXEL_SHADER)
}

void Direct3DObject::SaveLayout (UINT n)
{
	layoutN_ = n;
}

void Direct3DCamera::ok ()
{
	DEFAULT_OK_BLOCK
	if (wnd_ == nullptr)
		_EXC_N (NULL_WND, "D3D: Null window");
}

Direct3DCamera::Direct3DCamera (WindowClass* wnd, 
								float posX, float posY,  float posZ,
								float tgtX, float tgtY,  float tgtZ,
								float upX,  float upY,   float upZ,
								float fov,  float zNear, float zFar)
	try :
	wnd_				(wnd),
	position_			(XMVectorSet (posX, posY, posZ, 0.0f)),
	target_				(XMVectorSet (tgtX, tgtY, tgtZ, 0.0f)),
	up_					(XMVectorSet (upX,  upY,  upZ,  0.0f)),
	projectionSettings_ (fov, zNear, zFar),
	view_				(XMMatrixLookAtLH (position_, target_, up_)),
	projection_			(XMMatrixIdentity ())
{
	if (wnd == nullptr)
		_EXC_N (NULL_WND,
				"D3D: Cannot create camera over null window");

	projection_ = XMMatrixPerspectiveFovLH (projectionSettings_.x * 3.141592f,
											((float)wnd_->width ()) / wnd_->height (),
											projectionSettings_.y,
											projectionSettings_.z);
}
_END_EXCEPTION_HANDLING (CTOR)

void Direct3DCamera::Update ()
{
	BEGIN_EXCEPTION_HANDLING
	
	view_ = XMMatrixLookAtLH (position_, target_, up_);
	projection_ = XMMatrixPerspectiveFovLH (projectionSettings_.x * 3.141592f,
											((float)wnd_->width ()) / wnd_->height (),
											projectionSettings_.y,
											projectionSettings_.z);

	END_EXCEPTION_HANDLING (UPDATE_CAMERA)
}


const XMMATRIX& Direct3DCamera::GetView ()
{
	return view_;
}
const XMMATRIX& Direct3DCamera::GetProjection ()
{
	return projection_;
}
