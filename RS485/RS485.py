from flask import Flask, render_template

app = Flask(__name__)

# Trang chủ
@app.route('/')
def home():
    return '''
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>WebServer with Flask</title>
        <style>
            body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; }
            h1 { color: #4CAF50; }
        </style>
    </head>
    <body>
        <h1>Chào mừng đến với WebServer chạy cục bộ!</h1>
        <p>Đây là giao diện do Flask tạo ra.</p>
    </body>
    </html>
    '''

# Trang khác
@app.route('/about')
def about():
    return '<h1>Trang Giới thiệu</h1><p>Đây là một ví dụ khác.</p>'

if __name__ == '__main__':
    app.run(debug=True, port=5000)
