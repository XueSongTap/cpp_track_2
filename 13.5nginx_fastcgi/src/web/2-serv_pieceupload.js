const http = require('http')
const fs = require('fs')
const path = require('path')

const app = http.createServer((req, res) => {
  const { method, url } = req

  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type') // 可通过预检请求的Headers
  res.setHeader('Access-Control-Allow-Methods', 'POST') // 可通过预检请求的Method

  if (method === 'OPTIONS') {
    res.statusCode = 200
    res.end()
  }

  if (method === 'POST' && url === '/api/upload') {
    console.log('POST upload')
    const bufferList = []
    req.on('data', data => {
      bufferList.push(data)
    })
    req.on('end', () => {
      const body = Buffer.concat(bufferList)
      const boundary = `--${req.headers['content-type'].split('; ')[1].split('=')[1]}` // 获取分隔符
      console.log("handle body:\n", body.toString())  
      function bufferSplit (buffer, separator) { // 根据分隔符分隔数据
        let result = []
        let index = buffer.indexOf(separator)
        while (index !== -1) {
          const buf = buffer.slice(0, index)
          result.push(buf)
          buffer = buffer.slice(index + separator.length)
          index = buffer.indexOf(separator)
        }
        result.push(buffer)
        return result
      }
      
      // 1. 用分隔符切分数据
      const bodyArray = bufferSplit(body, boundary)
      // console.log(bodyArray.map(item => item.toString()))

      // 2. 删除数组头尾数据
      bodyArray.shift() // 去掉头 ''
      bodyArray.pop() // 去掉尾 '--\r\n'

      // 3. 将每一项数据头尾的的\r\n删除
      const bodyArrayNew = bodyArray.map(buffer => buffer.slice(2, buffer.length - 2))
      // console.log(bodyArrayNew.map(item => item.toString()))

      // 4.获取fileBlobInfo
      const fileBlobPath = path.resolve(__dirname, './fileBlob') // 定义文件小块的保存位置
      const filePath = path.resolve(__dirname, './fileSave') // 定义文件的保存位置
      const fileBlobInfo = {} // 定义文件小块信息对象
      bodyArrayNew.forEach(buffer => {
        const bufferInfo = bufferSplit(buffer, '\r\n\r\n') // 使用\r\n\r\n分割可得到包含参数信息和参数内容两项内容的数组

        const info = bufferInfo[0].toString() // info为字段信息，这是字符串类型数据，直接转换成字符串
        const data = bufferInfo[1] // data可能是普通数据也可能是文件内容
        const fieldName = info.split('; ')[1].split('=')[1].slice(1, -1)
        if (info.indexOf('\r\n') !== -1) {  // 若为文件内容，则数据中含有一个回车符\r\n，可以据此判断数据为文件还是为普通数据。
          const filename = info.split('; ')[2].split('\r\n')[0].split('=')[1].slice(1,-1)
          fileBlobInfo.filename = filename // 保存文件名到文件小块信息对象
          fileBlobInfo[fieldName] = data // 保存二进制数据到文件小块信息对象
        } else {
          fileBlobInfo[fieldName] = data.toString() // 保存其他字段到文件小块信息对象
        }
      })
      console.log(fileBlobInfo) // 此时fileBlobInfo下应该有如下三个字段，filename/fileBlob/fileBlobIndex

      // 5.保存分片
      fs.mkdirSync(path.resolve(fileBlobPath, fileBlobInfo.filename), { recursive: true }) // 以文件名为key创建文件夹
      fs.writeFileSync(path.resolve(fileBlobPath, fileBlobInfo.filename, fileBlobInfo.fileBlobIndex), fileBlobInfo.fileBlob) // 将分片写入文件夹
      res.statusCode = 200
      res.setHeader('Content-Type', 'application/json charset=utf-8')
      res.end('上传成功')
    })
  }

  if (method === 'POST' && url === '/api/merge') {
    console.log('POST merge')
    const fileBlobPath = path.resolve(__dirname, './fileBlob') // 定义文件小块的保存位置
    const filePath = path.resolve(__dirname, './fileSave') // 定义文件的保存位置
    const arrayBuffer = []
    req.on('data', data => {
      arrayBuffer.push(data)
    })
    req.on('end', () => {
      const body = JSON.parse(Buffer.concat(arrayBuffer)) // 获取请求正文内容

      fs.mkdirSync(filePath, { recursive: true }) // 创建文件夹
      const filenameList = fs.readdirSync(path.resolve(fileBlobPath, body.filename)) // 获取切块文件的文件名列表
      console.log("filenameList: ", filenameList) 
      filenameList.sort((a, b) => a - b).forEach(filename => {
        const file = fs.readFileSync(path.resolve(fileBlobPath, body.filename, filename)) // 分别获取切块文件
        fs.appendFileSync(path.resolve(filePath, body.filename), file) // 将切块文件数据添加到新的文件里
      })

      fs.rmSync(path.resolve(fileBlobPath, body.filename), { force: true, recursive: true })

      res.statusCode = 200
      res.setHeader('Content-Type', 'application/json; charset=utf-8')
      res.end('合并成功')
    })
  }

})

app.listen(3000, () => {
  console.log('listen 3000')
})