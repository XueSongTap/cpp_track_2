const http = require('http')
const fs = require('fs')
const path = require('path')
// 1.插件http 对象req请求, res应答
const app = http.createServer((req, res) => {
  const { method, url } = req
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type') // 可通过预检请求的Headers
  res.setHeader('Access-Control-Allow-Methods', 'POST') // 可通过预检请求的Method
  console.log("method:", method, ',url:', url)
  if (method === 'OPTIONS') {
    res.statusCode = 200
    res.end()
  }

  if (method === 'POST' && url === '/api/upload') {
    console.log("handle post: ", url)
    const bufferList = []
    req.on('data', data => {
      bufferList.push(data)  // 接受content数据
    })
    req.on('end', () => { //  content数据接受完毕
      const body = Buffer.concat(bufferList) // 拼接数据
      const boundary = `--${req.headers['content-type'].split('; ')[1].split('=')[1]}` // 获取分隔符
      console.log("handle body:\n", body.toString())  
      console.log("handle boundary: ", boundary)  // ------WebKitFormBoundary8qs4tkDdee3vqWCM
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

      // 4.获取最终结果
      const fileInfo = {
        filePath: '',
        // fileData: Buffer([]),
        fileData: Buffer.from([]) ,
        fieldInfo: {}
      }
      bodyArrayNew.forEach(buffer => {
        const bufferInfo = bufferSplit(buffer, '\r\n\r\n') // 使用\r\n\r\n分割可得到包含参数信息和参数内容两项内容的数组

        const info = bufferInfo[0].toString() // info为字段信息，这是字符串类型数据，直接转换成字符串
        const data = bufferInfo[1] // data可能是普通数据也可能是文件内容
        const fieldName = info.split('; ')[1].split('=')[1].slice(1, -1)
        if (info.indexOf('\r\n') !== -1) {  // 若为文件内容，则数据中含有一个回车符\r\n，可以据此判断数据为文件还是为普通数据。
          const filename = info.split('; ')[2].split('\r\n')[0].split('=')[1].slice(1,-1)
          fileInfo.filePath = path.resolve(__dirname, './file', filename)
          fileInfo.fileData = data
        } else {
          fileInfo.fieldInfo[fieldName] = data.toString()
        }
      })
      // 判断如果其他参数符合预期则储存文件内容
      if (fileInfo.fieldInfo.token === '123') {
        fs.writeFileSync(fileInfo.filePath, fileInfo.fileData)
        res.statusCode = 200
        res.setHeader('Content-Type', 'application/json charset=utf-8')
        res.end('上传成功')
        console.log('上传成功')
      }
    })
  }

})
// 2.监听对应的端口，这里是3000
app.listen(3000, () => {
  console.log('listen 3000')
})