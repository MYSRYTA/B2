//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �C�j�V�����C�U�[�@�i_path�ɂ͊g���q.json���܂߂�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template <class C>
JsonIO<C>::JsonIO ( const String &_path )
	: path{ _path }{}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �o�C�i���t�@�C���ւ̃p�X��ݒ肷�� �i�g���q.json���܂߂�j
//------------------------------------------------------------------------------------------------------------------------------------------------
template <class C>
void JsonIO<C>::setPath ( const String &_path ){
	this->path = _path;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �o�C�i���t�@�C���̏�������(�㏑��)�\�ȏ�Ԃɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <class C>
Bool JsonIO<C>::setWritable (){
	this->close ();
	const char* outfile = this->path.c_str ();
	this->fout.open ( outfile, std::ios::out );
	if ( this->fout ){
		this->mode = 1;
		return true;
	} else{
		this->mode = 0;
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �o�C�i���t�@�C���̓ǂݍ��݉\�ȏ�Ԃɂ���
//------------------------------------------------------------------------------------------------------------------------------------------------
template <class C>
Bool JsonIO<C>::setReadable (){
	this->close ();
	const char* outfile = this->path.c_str ();
	this->fin.open ( outfile, std::ios::in );
	if ( this->fin ){
		this->mode = 2;
		return true;
	} else{
		this->mode = 0;
		return false;
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �������� / �ǂݍ��ݏ�Ԃ��I������
//------------------------------------------------------------------------------------------------------------------------------------------------
template <class C>
void JsonIO<C>::close (){
	if ( this->mode == 1 ){ this->fout.close (); }
	if ( this->mode == 2 ){ this->fin.close (); }
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �N���X�f�[�^�̏������� (���O��setWritable()�����s���Ă��Ȃ��ꍇ�͖���)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <class C>
void JsonIO<C>::write ( const C &val ){
	if ( this->mode != 1 ){ return; }
	//�o�͎��̎󂯎M
	std::stringstream stream;
	{
		//�o�͗p�̌^�ɕ�����stream��o�^
		cereal::JSONOutputArchive jsonOutArchive ( stream );
		//������X�g���[����JSON�𓊂����Ă��炤
		jsonOutArchive ( cereal::make_nvp ( "JsonIO", val ) );
	}
	//�����o��
	this->fout << stream.str ();
	//����
	this->fout.close ();
	stream.clear ();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     �N���X�f�[�^�̓ǂݍ��� (���O��setReadable()�����s���Ă��Ȃ��ꍇ�͖���)
//------------------------------------------------------------------------------------------------------------------------------------------------
template <class C>
void JsonIO<C>::read ( OUT C &val ){
	if ( this->mode != 2 ){ return; }
	//���͂���镶����󂯎M
	std::stringstream stream;
	//���̓f�[�^��S��������stream�ɓ�����
	stream << this->fin.rdbuf ();
	{
		//bin�����[�h
		cereal::JSONInputArchive jsonInputArchive ( stream );
		//�f�R�[�h�����f�[�^���Z�b�g
		jsonInputArchive ( cereal::make_nvp ( "JsonIO", val ) );
	}
}